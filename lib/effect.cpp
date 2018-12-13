#include "flom/effect.hpp"

#include <utility>

#include <boost/qvm/quat_operations.hpp>

namespace flom {

Effect interpolate(double t, Effect const& a, Effect const& b) {
  Effect e;
  if (a.rotation && b.rotation) {
    e.rotation = interpolate(t, *a.rotation, *b.rotation);
  }
  if (a.translation && b.translation) {
    e.translation = interpolate(t, *a.translation, *b.translation);
  }
  return std::move(e);
}

Rotation interpolate(double t, Rotation const& a, Rotation const& b) {
  Rotation result;
  result.quat = qvm::slerp(a.quat, b.quat, t);
  return std::move(result);
}

Translation interpolate(double t, Translation const& a, Translation const& b) {
  Translation result;
  result.vec = a.vec + t * (b.vec - a.vec);
  return std::move(result);
}

}


