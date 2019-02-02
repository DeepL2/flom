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

#ifndef FLOM_TEST_PRINTERS_HPP
#define FLOM_TEST_PRINTERS_HPP

#include <flom/flom.hpp>

#include <boost/optional/optional_io.hpp>

namespace flom {

std::ostream &operator<<(std::ostream &os, const Rotation &v) {
  auto const &q = v.quaternion();
  os << "rotation(" << q.w() << ", " << q.x() << ", " << q.y() << ", " << q.z()
     << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const Location &v) {
  os << "location(" << v.vector() << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const Effector &v) {
  os << "effector(";
  if (v.location()) {
    os << *v.location() << ",";
  }
  if (v.rotation()) {
    os << *v.rotation() << ",";
  }
  os << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const EffectorDifference &v) {
  os << "effector_difference(";
  if (v.location()) {
    os << *v.location() << ",";
  }
  if (v.rotation()) {
    os << *v.rotation() << ",";
  }
  os << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, const FrameDifference &v) {
  os << "frame_difference(\n";
  os << "positions(";
  for (auto const &[j, p] : v.positions()) {
    os << j << ": " << p << ", ";
  }
  os << ")\neffectors(";
  for (auto const &[l, e] : v.effectors()) {
    os << l << ": " << e << ", ";
  }
  os << "))";
  return os;
}

std::ostream &operator<<(std::ostream &os, const Frame &v) {
  os << "frame(\n";
  os << "positions(";
  for (auto const &[j, p] : v.positions()) {
    os << j << ": " << p << ", ";
  }
  os << ")\neffectors(";
  for (auto const &[l, e] : v.effectors()) {
    os << l << ": " << e << ", ";
  }
  os << "))";
  return os;
}

std::ostream &operator<<(std::ostream &os, const Motion &v) {
  auto s = v.dump_json_string();
  os << s;
  return os;
}

} // namespace flom

#endif
