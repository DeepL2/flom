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

#ifndef FLOM_MOTION_IMPL_HPP
#define FLOM_MOTION_IMPL_HPP

#include "flom/frame.hpp"
#include "flom/motion.hpp"

#include "motion.pb.h"

#include <functional>
#include <map>
#include <numeric>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace flom {

template <typename K> std::size_t names_hash(const std::unordered_set<K> &s) {
  auto h{s.hash_function()};
  return std::accumulate(std::cbegin(s), std::cend(s),
                         static_cast<std::size_t>(0),
                         [&h](auto r, const auto &p) { return r ^ h(p); });
}

template <typename K, typename V>
std::size_t names_hash(const std::unordered_map<K, V> &m) {
  auto h{m.hash_function()};
  return std::accumulate(
      std::cbegin(m), std::cend(m), static_cast<std::size_t>(0),
      [&h](auto r, const auto &p) { return r ^ h(p.first); });
}

class Motion::Impl {
public:
  std::string model_id;
  LoopType loop;
  std::map<double, Frame> raw_frames;

  // keys of these two member must not be changed after construction
  const std::unordered_set<std::string> joint_names;
  const std::unordered_map<std::string, EffectorType> effector_types;
  std::unordered_map<std::string, EffectorWeight> effector_weights;

  // Hash of joint_names
  const std::size_t joints_hash;
  // Hash of keys of effector_types
  const std::size_t effectors_hash;

  Impl(const std::unordered_set<std::string> &joints,
       const std::unordered_map<std::string, EffectorType> &effectors,
       const std::string &model = "")
      : model_id(model), loop(LoopType::None), raw_frames(),
        joint_names(joints), effector_types(effectors),
        joints_hash(names_hash(joints)), effectors_hash(names_hash(effectors)) {
    this->effector_weights.reserve(effectors.size());
    for (const auto &[name, e] : effectors) {
      this->effector_weights.emplace(name, EffectorWeight{0.0, 0.0});
    }
    this->add_initial_frame();
  }

  void add_initial_frame();
  Frame new_keyframe() const noexcept;

  static Motion from_protobuf(proto::Motion const &);
  proto::Motion to_protobuf() const;

  bool is_valid() const;
  bool is_valid_frame(const Frame &) const;
};

} // namespace flom

#endif
