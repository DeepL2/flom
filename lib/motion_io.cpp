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

#include "flom/errors.hpp"
#include "flom/motion.hpp"
#include "flom/motion.impl.hpp"
#include "flom/proto_util.hpp"

#include "motion.pb.h"

#include <fstream>
#include <map>
#include <string>
#include <unordered_set>

#include <google/protobuf/util/json_util.h>

#include "nlohmann/json.hpp"

namespace flom {

Motion Motion::load(std::ifstream &f) {
  proto::Motion m;
  if (!m.ParseFromIstream(&f)) {
    throw errors::ParseError{};
  }

  return Motion::Impl::from_protobuf(m);
}

Motion Motion::load_json(std::ifstream &f) {
  std::string s;
  f >> s;
  return Motion::load_json_string(s);
}

Motion Motion::load_json_string(std::string const &s) {
  proto::Motion m;
  auto const status = google::protobuf::util::JsonStringToMessage(s, &m);
  if (!status.ok()) {
    throw errors::JSONLoadError(status.ToString());
  }
  return Motion::Impl::from_protobuf(m);
}

Motion Motion::Impl::from_protobuf(proto::Motion const &motion_proto) {
  std::unordered_set<std::string> joint_names;
  std::unordered_map<std::string, EffectorType> effector_types;
  std::transform(std::cbegin(motion_proto.effector_types()),
                 std::cend(motion_proto.effector_types()),
                 std::inserter(effector_types, std::end(effector_types)),
                 [](auto const &p) {
                   auto const &[link, type_proto] = p;
                   return std::make_pair(
                       link, proto_util::unpack_effector_type(type_proto));
                 });
  auto const &init_pos = motion_proto.frames(0).positions();
  std::transform(std::cbegin(init_pos), std::cend(init_pos),
                 std::inserter(joint_names, std::end(joint_names)),
                 [](auto const &p) { return p.first; });

  Motion m(joint_names, effector_types, motion_proto.model_id());
  if (motion_proto.loop() == proto::Motion::Loop::Motion_Loop_Wrap) {
    m.impl->loop = LoopType::Wrap;
  } else if (motion_proto.loop() == proto::Motion::Loop::Motion_Loop_None) {
    m.impl->loop = LoopType::None;
  }
  for (auto const &frame_proto : motion_proto.frames()) {
    auto &frame = m.impl->raw_frames[frame_proto.t()];
    auto const &positions_proto = frame_proto.positions();
    std::copy(std::cbegin(positions_proto), std::cend(positions_proto),
              std::inserter(frame.positions, std::end(frame.positions)));
    auto const &effectors_proto = frame_proto.effectors();
    std::transform(std::cbegin(effectors_proto), std::cend(effectors_proto),
                   std::inserter(frame.effectors, std::end(frame.effectors)),
                   [](auto const &p) {
                     auto const &effect_proto = p.second;
                     Effector e;
                     if (effect_proto.has_location()) {
                       e.location = proto_util::unpack_location(
                           effect_proto.location().value());
                     }
                     if (effect_proto.has_rotation()) {
                       e.rotation = proto_util::unpack_rotation(
                           effect_proto.rotation().value());
                     }
                     // TODO: Delete copy
                     return std::make_pair(p.first, e);
                   });
  }

  if (!m.is_valid()) {
    throw errors::InvalidMotionError{"while loading parsed motion data"};
  }

  // copy occurs...
  return m;
}
void Motion::dump(std::ofstream &f) const {
  auto const m = this->impl->to_protobuf();
  if (!m.SerializeToOstream(&f)) {
    throw errors::SerializationError{};
  }
}

void Motion::dump_json(std::ofstream &f) const {
  f << this->dump_json_string();
}

std::string Motion::dump_json_string() const {
  std::string s;
  auto const m = this->impl->to_protobuf();
  google::protobuf::util::JsonPrintOptions opt;
  opt.always_print_primitive_fields = true;
  auto const status = google::protobuf::util::MessageToJsonString(m, &s, opt);
  if (!status.ok()) {
    throw errors::JSONDumpError(status.ToString());
  }
  return s;
}

proto::Motion Motion::Impl::to_protobuf() const {
  if (!this->is_valid()) {
    throw errors::InvalidMotionError{
        "converting motion data before serializaion"};
  }

  proto::Motion m;
  m.set_model_id(this->model_id);
  if (this->loop == LoopType::Wrap) {
    m.set_loop(proto::Motion::Loop::Motion_Loop_Wrap);
  } else if (this->loop == LoopType::None) {
    m.set_loop(proto::Motion::Loop::Motion_Loop_None);
  }
  for (auto const &[link, type] : this->effector_types) {
    proto_util::pack_effector_type(type, &(*m.mutable_effector_types())[link]);
  }
  for (auto const &[t, frame] : this->raw_frames) {
    auto *frame_proto = m.add_frames();
    frame_proto->set_t(t);
    // TODO: Use <algorithm> (e.g. std::copy)
    for (auto const &[joint, change] : frame.positions) {
      (*frame_proto->mutable_positions())[joint] = change;
    }
    for (auto const &[link, effect] : frame.effectors) {
      proto::Effector e;
      if (effect.location) {
        proto_util::pack_location(*effect.location,
                                  e.mutable_location()->mutable_value());
      }
      if (effect.rotation) {
        proto_util::pack_rotation(*effect.rotation,
                                  e.mutable_rotation()->mutable_value());
      }
      (*frame_proto->mutable_effectors())[link] = e;
    }
  }

  return m;
}

Motion Motion::load_legacy_json(std::ifstream &s) {
  using json = nlohmann::json;

  json json_data;
  s >> json_data;

  std::unordered_set<std::string> joint_names, effector_names;
  {
    auto const &init_frame = json_data["frames"][0];
    auto const positions = init_frame["position"];
    // TODO: Use <algorithm> (e.g. std::copy)
    for (auto it = std::cbegin(positions); it != std::cend(positions); ++it) {
      joint_names.insert(it.key());
    }
    auto const effectors = init_frame["effector"];
    for (auto it = std::cbegin(effectors); it != std::cend(effectors); ++it) {
      effector_names.insert(it.key());
    }
  }

  Motion m(joint_names, effector_names, json_data["model"]);
  {
    auto loop_type = json_data["loop"];
    if (loop_type == "wrap") {
      m.impl->loop = LoopType::Wrap;
    } else if (loop_type == "none") {
      m.impl->loop = LoopType::None;
    } else {
      throw std::runtime_error("Unknown loop type");
    }
  }
  {
    auto const frames = json_data["frames"];
    for (auto const &frame : frames) {
      Frame f;
      const double time = frame["timepoint"];
      auto const positions = frame["position"];
      // TODO: Use <algorithm> (e.g. std::copy)
      for (auto it = std::cbegin(positions); it != std::cend(positions); ++it) {
        f.positions[it.key()] = it.value();
      }
      auto const effectors = frame["effector"];
      for (auto it = std::cbegin(effectors); it != std::cend(effectors); ++it) {
        Effector e;
        auto const effect_data = it.value();
        if (effect_data.count("location") != 0) {
          Location trans;
          auto const trans_data = effect_data["location"];
          auto const &value = trans_data["value"];
          // Only last frame is used, so not good code, but this is for
          // importing legacy format anyway (?)
          if (trans_data["space"] == "world") {
            m.impl->effector_types[it.key()].location = CoordinateSystem::World;
          } else if (trans_data["space"] == "local") {
            m.impl->effector_types[it.key()].location = CoordinateSystem::Local;
          }
          trans.vec = boost::qvm::vec<double, 3>{value[0], value[1], value[2]};
          trans.weight = trans_data["weight"];
          e.location = std::move(trans);
        }

        if (effect_data.count("rotation") != 0) {
          Rotation rot;
          auto const rot_data = effect_data["rotation"];
          auto const &value = rot_data["value"];
          // Only last frame is used, so not good code, but this is for
          // importing legacy format anyway (?)
          if (rot_data["space"] == "world") {
            m.impl->effector_types[it.key()].rotation = CoordinateSystem::World;
          } else if (rot_data["space"] == "local") {
            m.impl->effector_types[it.key()].rotation = CoordinateSystem::Local;
          }
          rot.quat =
              boost::qvm::quat<double>{value[0], value[1], value[2], value[3]};
          rot.weight = rot_data["weight"];
          e.rotation = std::move(rot);
        }

        f.effectors[it.key()] = e;
      }
      m.impl->raw_frames[time] = std::move(f);
    }
  }

  if (!m.is_valid()) {
    throw errors::InvalidMotionError{"while loading legacy motion data"};
  }

  // copy occurs...
  return m;
}

} // namespace flom
