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

#ifndef FLOM_RANGE_HPP
#define FLOM_RANGE_HPP

#include "flom/errors.hpp"
#include "flom/frame.hpp"
#include "flom/motion.hpp"

#include <iterator>
#include <map>
#include <memory>

namespace flom {

// using snake_case, following customs of iterator naming
class frame_iterator {
  friend bool operator==(const frame_iterator &,
                         const frame_iterator &) noexcept;

public:
  using iterator_category = std::input_iterator_tag;
  using value_type = Frame;
  using difference_type = double;
  using pointer = Frame *;
  using reference = Frame &;

private:
  class Impl;
  std::unique_ptr<Impl> impl;

  bool is_end = false;

public:
  frame_iterator() noexcept;
  frame_iterator(Motion const &motion, double fps) noexcept;

  frame_iterator(const frame_iterator &);
  frame_iterator(frame_iterator &&);
  frame_iterator &operator=(const frame_iterator &);
  frame_iterator &operator=(frame_iterator &&);

  ~frame_iterator();

  // This is InputIterator because operator* doesn't return reference
  value_type operator*() const;

  frame_iterator &operator++() noexcept;
  frame_iterator operator++(int) noexcept;

  frame_iterator &operator--() noexcept;
  frame_iterator operator--(int) noexcept;

  double current_time() const noexcept;
};

frame_iterator::difference_type operator-(const frame_iterator &,
                                          const frame_iterator &) noexcept;
bool operator==(const frame_iterator &, const frame_iterator &) noexcept;
bool operator!=(const frame_iterator &, const frame_iterator &) noexcept;

class FrameRange {
public:
  using value_type = Frame;
  using iterator = frame_iterator;

private:
  Motion const &motion;
  double fps;

public:
  FrameRange() = delete;
  FrameRange(Motion const &motion_, double fps_) : motion(motion_), fps(fps_) {}
  FrameRange(const FrameRange &) = default;
  FrameRange(FrameRange &&) = default;
  FrameRange &operator=(const FrameRange &) = default;
  FrameRange &operator=(FrameRange &&) = default;

  iterator begin() noexcept { return {this->motion, this->fps}; }
  iterator end() noexcept { return {}; }
};

class CheckedFrameRef {
public:
  using reference_type = Frame &;

  CheckedFrameRef(reference_type value_, const Motion *motion_)
      : value(value_), motion(motion_) {}

  CheckedFrameRef &operator=(const Frame &frame) & {
    if (!this->motion->is_valid_frame(frame)) {
      throw errors::InvalidFrameError{"in CheckedFrameWrapper"};
    }
    this->value = frame;
  }

  operator reference_type() const noexcept { return this->value; }

private:
  reference_type value;
  const Motion *motion;
};

class keyframe_iterator {
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = Frame;
  using difference_type = std::ptrdiff_t;
  using pointer = Frame *;
  using reference = Frame &;

  using base_iterator = std::map<double, Frame>::iterator;

private:
  friend bool operator==(const keyframe_iterator &,
                         const keyframe_iterator &) noexcept;

  base_iterator it;
  const Motion *motion;

public:
  keyframe_iterator() noexcept : it(), motion() {}
  explicit keyframe_iterator(base_iterator it_, const Motion &motion_) noexcept
      : it(it_), motion(&motion_) {}

  keyframe_iterator(const keyframe_iterator &) = default;
  keyframe_iterator(keyframe_iterator &&) = default;
  keyframe_iterator &operator=(const keyframe_iterator &) = default;
  keyframe_iterator &operator=(keyframe_iterator &&) = default;

  const value_type &operator*() const;
  CheckedFrameRef operator*();

  const value_type &operator->() const;
  CheckedFrameRef operator->();

  keyframe_iterator &operator++() noexcept;
  keyframe_iterator operator++(int) noexcept;

  keyframe_iterator &operator--() noexcept;
  keyframe_iterator operator--(int) noexcept;
};

bool operator==(const keyframe_iterator &, const keyframe_iterator &) noexcept;
bool operator!=(const keyframe_iterator &, const keyframe_iterator &) noexcept;

class KeyframeRange {
public:
  using value_type = Frame;
  using iterator = keyframe_iterator;
  using base_iterator = typename std::map<double, Frame>::iterator;

private:
  base_iterator begin_it;
  base_iterator end_it;
  const Motion &motion;

public:
  KeyframeRange() = delete;
  KeyframeRange(base_iterator begin_, base_iterator end_, const Motion &motion_)
      : begin_it(begin_), end_it(end_), motion(motion_) {}
  KeyframeRange(const KeyframeRange &) = default;
  KeyframeRange(KeyframeRange &&) = default;
  KeyframeRange &operator=(const KeyframeRange &) = default;
  KeyframeRange &operator=(KeyframeRange &&) = default;

  iterator begin() noexcept { return iterator{this->begin_it, this->motion}; }
  iterator end() noexcept { return iterator{this->end_it, this->motion}; }
};

} // namespace flom

#endif
