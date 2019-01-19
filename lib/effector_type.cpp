#include "flom/effector_type.hpp"

namespace flom {

EffectorType::EffectorType(compat::optional<CoordinateSystem> location,
                           compat::optional<CoordinateSystem> rotation)
    : location_(location), rotation_(rotation) {}

compat::optional<CoordinateSystem> EffectorType::location() const {
  return this->location_;
}
compat::optional<CoordinateSystem> EffectorType::rotation() const {
  return this->rotation_;
}

void EffectorType::set_location(compat::optional<CoordinateSystem> location) {
  this->location_ = location;
}
void EffectorType::clear_location() { this->set_location(std::nullopt); }
void EffectorType::set_rotation(compat::optional<CoordinateSystem> rotation) {
  this->rotation_ = rotation;
}
void EffectorType::clear_rotation() { this->set_rotation(std::nullopt); }

Effector EffectorType::new_effector() const {
  Effector e;
  if (this->location()) {
    e.set_location(Location{});
  }
  if (this->rotation()) {
    e.set_rotation(Rotation{});
  }
  return e;
}

bool EffectorType::is_compatible(const Effector &e) const {
  bool loc_v = static_cast<bool>(e.location());
  bool loc_t = static_cast<bool>(this->location());
  bool rot_v = static_cast<bool>(e.rotation());
  bool rot_t = static_cast<bool>(this->rotation());
  return loc_v == loc_t && rot_v == rot_t;
}

bool operator==(const EffectorType &v1, const EffectorType &v2) {
  return v1.location() == v2.location() && v1.rotation() == v2.rotation();
}

} // namespace flom
