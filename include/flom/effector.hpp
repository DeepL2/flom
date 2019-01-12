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

#ifndef FLOM_EFFECTOR_HPP
#define FLOM_EFFECTOR_HPP

#include <optional>
#include <type_traits>

#include <boost/operators.hpp>
#include <boost/qvm/quat.hpp>
#include <boost/qvm/quat_operations.hpp>
#include <boost/qvm/vec.hpp>
#include <boost/qvm/vec_operations.hpp>

namespace flom {

namespace qvm = boost::qvm;

class Location
    : boost::addable<
          Location,
          boost::subtractable<
              Location,
              boost::equality_comparable<
                  Location, boost::multipliable<Location, std::size_t>>>> {
public:
  using value_type = qvm::vec<double, 3>;

private:
  value_type vector_;

public:
  Location();
  explicit Location(const value_type &);

  const value_type &vector() const;
  void set_vector(const value_type &);

  Location &operator+=(const Location &);
  Location &operator-=(const Location &);
  Location &operator*=(std::size_t);
};

bool operator==(const Location &, const Location &);
bool almost_equal(const Location &, const Location &);

struct Rotation
    : boost::addable<
          Rotation,
          boost::subtractable<
              Rotation,
              boost::equality_comparable<
                  Rotation, boost::multipliable<Rotation, std::size_t>>>> {
public:
  using value_type = qvm::quat<double>;

private:
  value_type quat_;

public:
  Rotation();
  explicit Rotation(const value_type &);

  const value_type &quaternion() const;
  void set_quaternion(const value_type &);

  Rotation &operator+=(const Rotation &);
  Rotation &operator-=(const Rotation &);
  Rotation &operator*=(std::size_t);
};

bool operator==(const Rotation &, const Rotation &);
bool almost_equal(const Rotation &, const Rotation &);

struct Effector;

class EffectorDifference
    : boost::addable<
          EffectorDifference,
          boost::equality_comparable<
              EffectorDifference,
              boost::multipliable<EffectorDifference, std::size_t>>> {
private:
  std::optional<Location> location_;
  std::optional<Rotation> rotation_;

public:
  EffectorDifference(const Effector &, const Effector &);

  EffectorDifference() = delete;

  EffectorDifference(const EffectorDifference &) = default;
  EffectorDifference(EffectorDifference &&) = default;

  EffectorDifference &operator=(const EffectorDifference &) = default;
  EffectorDifference &operator=(EffectorDifference &&) = default;

  const std::optional<Location> &location() const &;
  std::optional<Location> location() &&;

  const std::optional<Rotation> &rotation() const &;
  std::optional<Rotation> rotation() &&;

  EffectorDifference &operator*=(std::size_t);
  EffectorDifference &operator+=(const EffectorDifference &);

  bool is_compatible(const EffectorDifference &) const;
};

bool operator==(const EffectorDifference &, const EffectorDifference &);
bool almost_equal(const EffectorDifference &, const EffectorDifference &);

struct Effector : boost::addable<Effector, EffectorDifference> {
private:
  std::optional<Location> location_;
  std::optional<Rotation> rotation_;

public:
  Effector();
  Effector(const std::optional<Location> &, const std::optional<Rotation> &);

  const std::optional<Location> &location() const &;
  std::optional<Location> location() &&;

  void set_location(const std::optional<Location> &);
  void clear_location();

  const std::optional<Rotation> &rotation() const &;
  std::optional<Rotation> rotation() &&;

  void set_rotation(const std::optional<Rotation> &);
  void clear_rotation();

  Effector new_compatible_effector() const;
  bool is_compatible(const Effector &) const;
  bool is_compatible(const EffectorDifference &) const;

  Effector &operator+=(const EffectorDifference &);
};

bool operator==(const Effector &, const Effector &);
bool operator!=(const Effector &, const Effector &);
bool almost_equal(const Effector &, const Effector &);
EffectorDifference operator-(const Effector &, const Effector &);

bool almost_equal(double, double);

} // namespace flom

#endif
