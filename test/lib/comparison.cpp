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

#include <flom/effector.hpp>
#include <flom/frame.hpp>
#include <flom/loose_compare.hpp>

#include <boost/qvm/quat.hpp>
#include <boost/qvm/quat_access.hpp>
#include <boost/qvm/quat_operations.hpp>
#include <boost/qvm/vec.hpp>
#include <boost/qvm/vec_access.hpp>
#include <boost/qvm/vec_operations.hpp>
#include <boost/qvm/vec_traits_array.hpp>

#include "comparison.hpp"

// loose comparison functions to use in tests.

namespace flom {

namespace testing {

bool almost_equal(double a, double b) { return loose_compare(a, b); }

bool almost_equal(const qvm::vec<double, 3> &a,
                         const qvm::vec<double, 3> &b) {
  return boost::qvm::cmp(a, b,
                         [](auto e1, auto e2) { return almost_equal(e1, e2); });
}

bool almost_equal(const qvm::quat<double> &a,
                         const qvm::quat<double> &b) {
  return boost::qvm::cmp(a, b,
                         [](auto e1, auto e2) { return almost_equal(e1, e2); });
}

bool almost_equal(const Location &v1, const Location &v2) {
  return almost_equal(v1.vector(), v2.vector());
}

bool almost_equal(const Rotation &v1, const Rotation &v2) {
  return almost_equal(v1.quaternion(), v2.quaternion());
}

bool almost_equal(const Effector &v1, const Effector &v2) {
  // TODO: Refactor: Remove mutable variable
  const bool l =
      static_cast<bool>(v1.location()) == static_cast<bool>(v2.location());
  const bool r =
      static_cast<bool>(v1.rotation()) == static_cast<bool>(v2.rotation());
  if (!l || !r)
    return false;
  bool result = true;
  if (v1.location())
    result = result && almost_equal(*v1.location(), *v2.location());
  if (v1.rotation())
    result = result && almost_equal(*v1.rotation(), *v2.rotation());
  return result;
}

bool almost_equal(const Frame &v1, const Frame &v2) {
  auto p = std::all_of(std::cbegin(v1.positions()), std::cend(v1.positions()),
                       [&v2](auto const &pair) {
                         auto const &[joint, pos1] = pair;
                         auto const pos2 = v2.positions().at(joint);
                         return almost_equal(pos1, pos2);
                       });
  auto e = std::all_of(std::cbegin(v1.effectors()), std::cend(v1.effectors()),
                       [&v2](auto const &pair) {
                         auto const &[link, e1] = pair;
                         auto const e2 = v2.effectors().at(link);
                         return almost_equal(e1, e2);
                       });
  return p && e;
}

bool almost_equal(const EffectorDifference &v1,
                         const EffectorDifference &v2) {
  const bool l =
      static_cast<bool>(v1.location()) == static_cast<bool>(v2.location());
  const bool r =
      static_cast<bool>(v1.rotation()) == static_cast<bool>(v2.rotation());
  if (!l || !r)
    return false;
  bool result = true;
  if (v1.location())
    result = result && almost_equal(*v1.location(), *v2.location());
  if (v1.rotation())
    result = result && almost_equal(*v1.rotation(), *v2.rotation());
  return result;
}

bool almost_equal(const FrameDifference &v1, const FrameDifference &v2) {
  auto p = std::all_of(std::cbegin(v1.positions()), std::cend(v1.positions()),
                       [&v2](auto const &pair) {
                         auto const &[joint, pos1] = pair;
                         auto const pos2 = v2.positions().at(joint);
                         return almost_equal(pos1, pos2);
                       });
  auto e = std::all_of(std::cbegin(v1.effectors()), std::cend(v1.effectors()),
                       [&v2](auto const &pair) {
                         auto const &[link, e1] = pair;
                         auto const e2 = v2.effectors().at(link);
                         return almost_equal(e1, e2);
                       });
  return p && e;
}

} // namespace testing

} // namespace flom
