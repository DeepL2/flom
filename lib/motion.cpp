//
// Copyright 2018 coord.e
//
// This file is part of Flom.
//
// Flom is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Flom is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Flom.  If not, see <http://www.gnu.org/licenses/>.
//

#include "flom/motion.hpp"
#include "flom/errors.hpp"
#include "flom/interpolation.hpp"
#include "flom/loose_compare.hpp"
#include "flom/motion.impl.hpp"
#include "flom/range.hpp"

#include "motion.pb.h"

#include <boost/range/adaptors.hpp>

#include <cmath>
#include <string>

namespace flom {

Motion::Motion(
    const std::unordered_set<std::string> &joint_names,
    const std::unordered_map<std::string, EffectorType> &effector_types,
    const std::string &model)
    : impl(std::make_unique<Motion::Impl>(joint_names, effector_types, model)) {
}
Motion::Motion(Motion const &m)
    : impl(std::make_unique<Motion::Impl>(*m.impl)) {}

Motion::~Motion() {}

bool Motion::is_valid() const { return this->impl && this->impl->is_valid(); }
bool Motion::is_valid_frame(const Frame &f) const {
  return this->impl->is_valid_frame(f);
}

Frame Motion::frame_at(double t) const {
  if (std::isnan(t) || t < 0) {
    throw errors::InvalidTimeError(t);
  }
  auto const [l, u] = this->impl->raw_frames.equal_range(t);
  if (l->first == t) {
    // found a frame with exactly same time
    return l->second; // causes copy...
  } else if (u == this->impl->raw_frames.end()) {
    // Out of frames
    if (this->impl->loop == LoopType::Wrap) {
      auto const &last = std::next(l, -1);
      auto const motion_length = last->first;
      if (motion_length == 0) {
        // only one frame with t == 0
        return last->second;
      }

      auto const skip_episode = static_cast<unsigned>(t / motion_length);
      auto const trailing_t = t - skip_episode * motion_length;
      auto const init_frame = this->impl->raw_frames.begin()->second;
      auto const diff = last->second - init_frame;
      return this->frame_at(trailing_t) + diff * skip_episode;
    } else {
      throw errors::OutOfFramesError(t);
    }
  } else {
    // Between two frames -> interpolate
    auto const t1 = std::next(l, -1)->first;
    auto const t2 = u->first;
    auto const &f1 = std::next(l, -1)->second;
    auto const &f2 = u->second;
    return interpolate((t - t1) / (t2 - t1), f1, f2);
  }
}

FrameRange Motion::frames(double fps) const { return FrameRange{*this, fps}; }

bool Motion::is_in_range_at(double t) const {
  if (this->impl->loop == LoopType::Wrap) {
    return true;
  } else {
    auto const last_t = std::next(this->impl->raw_frames.end(), -1)->first;
    return t <= last_t;
  }
}

void Motion::insert_keyframe(double t, const Frame &frame) {
  if (!this->impl->is_valid_frame(frame)) {
    throw errors::InvalidFrameError{"during keyframe insertion"};
  }
  this->impl->raw_frames[t] = frame;
}

void Motion::delete_keyframe(double t, bool loose) {
  if (t == 0 || (loose && loose_compare(t, 0))) {
    throw errors::InitKeyframeError{};
  }

  if (this->impl->raw_frames.erase(t) != 0) {
    return;
  }
  if (!loose) {
    throw errors::KeyframeNotFoundError{t};
  }

  // loose mode - find closest key
  auto const lower = this->impl->raw_frames.lower_bound(t);
  auto const begin = std::cbegin(this->impl->raw_frames);

  std::remove_const_t<decltype(begin)> it;
  if (lower != begin) {
    auto const previous = std::prev(lower);
    if ((t - previous->first) < (lower->first - t)) {
      it = previous;
    } else {
      it = lower;
    }
  } else {
    it = lower;
  }

  if (!loose_compare(t, it->first)) {
    throw errors::KeyframeNotFoundError{t};
  }

  this->impl->raw_frames.erase(it);
}

KeyframeRange Motion::keyframes() {
  return {std::begin(this->impl->raw_frames), std::end(this->impl->raw_frames),
          *this};
}

ConstKeyframeRange Motion::const_keyframes() const {
  return {std::cbegin(this->impl->raw_frames),
          std::cend(this->impl->raw_frames)};
}

void Motion::clear_keyframes() {
  auto const first = std::cbegin(this->impl->raw_frames);
  auto const end = std::cend(this->impl->raw_frames);
  this->impl->raw_frames.erase(std::next(first, 1), end);
}

LoopType Motion::loop() const { return this->impl->loop; }

void Motion::set_loop(LoopType loop) { this->impl->loop = loop; }

std::string Motion::model_id() const { return this->impl->model_id; }

void Motion::set_model_id(std::string const &model_id) {
  this->impl->model_id = model_id;
}

EffectorType Motion::effector_type(const std::string &name) const {
  return this->impl->effector_types.at(name);
}

EffectorWeight Motion::effector_weight(const std::string &name) const {
  return this->impl->effector_weights.at(name);
}

void Motion::set_effector_weight(const std::string &name,
                                 EffectorWeight weight) {
  this->impl->effector_weights.at(name) = weight;
}

double Motion::length() const {
  return std::next(this->impl->raw_frames.end(), -1)->first;
}

Frame Motion::new_keyframe() const { return this->impl->new_keyframe(); }

Frame Motion::Impl::new_keyframe() const noexcept {
  std::unordered_map<std::string, double> p;
  std::unordered_map<std::string, Effector> e;

  p.reserve(this->joint_names.size());
  for (const auto &name : this->joint_names) {
    p.emplace(name, 0.0);
  }

  e.reserve(this->effector_types.size());
  for (const auto &[name, type] : this->effector_types) {
    e.emplace(name, type.new_effector());
  }

  return {p, e};
}

void Motion::Impl::add_initial_frame() {
  assert(this->raw_frames.size() == 0 && "raw_frames already initialized");

  this->raw_frames.emplace(0.0, this->new_keyframe());
}

bool Motion::Impl::is_valid() const {
  // for internal use
  //
  // Arbitrary flom::Motion object,
  // which is constructed only using public interface,
  // must not be marked as invalid by this method.
  //
  if (this->raw_frames.size() == 0 || this->raw_frames.begin()->first != 0) {
    return false;
  }

  for (auto const &[t, frame] : this->raw_frames) {
    if (!this->is_valid_frame(frame)) {
      return false;
    }
  }
  return true;
}

bool Motion::Impl::is_valid_frame(const Frame &frame) const {
  auto const &p = frame.positions();
  auto const &e = frame.effectors();

  if (names_hash(p) != this->joints_hash ||
      names_hash(e) != this->effectors_hash) {
    return false;
  }
  for (auto const &[name, type] : this->effector_types) {
    if (!type.is_compatible(e.at(name))) {
      return false;
    }
  }
  return true;
}

KeyRange<std::string> Motion::joint_names() const {
  return this->impl->joint_names;
}

KeyRange<std::string> Motion::effector_names() const {
  return this->impl->effector_types | boost::adaptors::map_keys;
}

bool operator==(const Motion &m1, const Motion &m2) {
  return m1.impl->model_id == m2.impl->model_id &&
         m1.impl->loop == m2.impl->loop &&
         m1.impl->raw_frames == m2.impl->raw_frames &&
         m1.impl->effector_types == m2.impl->effector_types &&
         m1.impl->effector_weights == m2.impl->effector_weights;
}

bool operator!=(const Motion &m1, const Motion &m2) { return !(m1 == m2); }

} // namespace flom
