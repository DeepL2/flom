#ifndef FLOM_PROTO_UTIL_HPP
#define FLOM_PROTO_UTIL_HPP

#include "motion.pb.h"
#include "frame.pb.h"
#include "rotation.pb.h"
#include "translation.pb.h"

namespace flom::proto_util {

void pack_vec3(boost::qvm::vec<double, 3> const&, proto::Vec3*);
void pack_translation(Translation const&, proto::Translation*);

boost::qvm::vec<double, 3> unpack_vec3(proto::Vec3 const&);
Translation unpack_translation(proto::Translation const&);

void pack_quat(boost::qvm::quat<double> const&, proto::Quaternion*);
void pack_rotation(Rotation const&, proto::Rotation*);

boost::qvm::quat<double> unpack_quat(proto::Quaternion const&);
Rotation unpack_rotation(proto::Rotation const&);

}

#endif
