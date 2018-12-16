#ifndef FLOM_TEST_GENERATORS_HPP
#define FLOM_TEST_GENERATORS_HPP

#include <boost/qvm/quat.hpp>
#include <boost/qvm/quat_operations.hpp>
#include <boost/qvm/vec.hpp>

#include <rapidcheck.h>

#include <flom/effector.hpp>
#include <flom/frame.hpp>
#include <flom/motion.hpp>
#include <flom/constants.hpp>

#include <unordered_map>

namespace rc {

template <> struct Arbitrary<boost::qvm::vec<double, 3>> {
  static auto arbitrary() -> decltype(auto) {
    return gen::construct<boost::qvm::vec<double, 3>>(gen::arbitrary<double>(),
                                                      gen::arbitrary<double>(),
                                                      gen::arbitrary<double>());
  }
};

template <> struct Arbitrary<boost::qvm::quat<double>> {
  static auto arbitrary() -> decltype(auto) {
    const int half_pi_100 = flom::constants::pi<double> / 2 * 100;
    return gen::apply(
        [](int x, int y, int z) {
          auto q = boost::qvm::quat<double>();
          boost::qvm::set_rotx(q, static_cast<double>(x) / 100);
          boost::qvm::set_roty(q, static_cast<double>(y) / 100);
          boost::qvm::set_rotz(q, static_cast<double>(z) / 100);
          return q;
        },
        gen::inRange(-half_pi_100, half_pi_100), gen::inRange(-half_pi_100, half_pi_100),
        gen::inRange(-half_pi_100, half_pi_100));
  };
};

template <> struct Arbitrary<flom::Location> {
  static auto arbitrary() -> decltype(auto) {
    return gen::apply(
        [](unsigned weight,
           boost::qvm::vec<double, 3> const &v) {
          flom::Location l;
          l.weight = static_cast<double>(weight) / 100;
          // TODO: Evil, but will be deleted!
          l.coord_system = flom::CoordinateSystem::World;
          l.vec = v;
          return l;
        },
        gen::inRange<unsigned>(0, 100),
        gen::arbitrary<boost::qvm::vec<double, 3>>());
  }
};

template <> struct Arbitrary<flom::Rotation> {
  static auto arbitrary() -> decltype(auto) {
    return gen::apply(
        [](unsigned weight,
           boost::qvm::quat<double> const &q) {
          flom::Rotation r;
          r.weight = static_cast<double>(weight) / 100;
          // TODO: Evil, but will be deleted!
          r.coord_system = flom::CoordinateSystem::World;
          r.quat = q;
          return r;
        },
        gen::inRange<unsigned>(0, 100),
        gen::arbitrary<boost::qvm::quat<double>>());
  }
};

template <> struct Arbitrary<flom::Effector> {
  static auto arbitrary() -> decltype(auto) {
    return gen::apply(
        [](Maybe<flom::Location> const &l, Maybe<flom::Rotation> const &r) {
          flom::Effector e;
          if (l) {
            e.location = *l;
          }
          if (r) {
            e.rotation = *r;
          }
          return e;
        },
        gen::maybe(gen::arbitrary<flom::Location>()),
        gen::maybe(gen::arbitrary<flom::Rotation>()));
  }
};

template <> struct Arbitrary<flom::Frame> {
  static auto arbitrary() -> decltype(auto) {
    return gen::build(
        gen::construct<flom::Frame>(),
        gen::set(&flom::Frame::positions,
                 gen::arbitrary<std::unordered_map<std::string, double>>()),
        gen::set(
            &flom::Frame::effectors,
            gen::arbitrary<std::unordered_map<std::string, flom::Effector>>()));
  }
};

template <> struct Arbitrary<flom::Motion> {
  static auto arbitrary() -> decltype(auto) {
    return gen::apply(
        [](std::string const &model_id, flom::LoopType loop, double fps, auto const& frames) {
          flom::Motion m(model_id);
          m.set_loop(loop);
          unsigned i = 0;
          for (auto const& [p, e] : frames) {
            auto& f = m.get_or_insert_frame(fps * i++);
            std::copy(std::cbegin(p), std::cend(p), std::inserter(f.positions, std::end(f.positions)));
            std::copy(std::cbegin(e), std::cend(e), std::inserter(f.effectors, std::end(f.effectors)));
          }
          return m;
        },
        gen::nonEmpty<std::string>(),
        gen::element(flom::LoopType::None, flom::LoopType::Wrap),
        gen::nonZero<double>(),
        gen::nonEmpty(gen::mapcat(
           gen::pair(
            gen::arbitrary<std::vector<std::string>>(),
            gen::arbitrary<std::vector<std::string>>()
          ),
          [] (auto const& t) {
            auto const& [joints, effectors] = t;
            auto j = gen::exec([&joints = joints]() {
              std::unordered_map<std::string, double> nj;
              std::transform(std::cbegin(joints), std::cend(joints), std::inserter(nj, std::end(nj)), [](auto&& j) {
                  const int half_pi_100 = flom::constants::pi<double> / 2 * 100;
                  return std::make_pair(j, static_cast<double>(*gen::inRange(-half_pi_100, half_pi_100)) / 100);
              });
              return nj;
            });
            auto eg = gen::build(
                gen::construct<flom::Effector>(),
                gen::set(&flom::Effector::location, gen::arbitrary<flom::Location>()),
                gen::set(&flom::Effector::rotation, gen::arbitrary<flom::Rotation>()));
            auto e = gen::exec([&effectors = effectors, eg]() {
              std::unordered_map<std::string, flom::Effector> nj;
              std::transform(std::cbegin(effectors), std::cend(effectors), std::inserter(nj, std::end(nj)), [&eg](auto&& j) {
                  return std::make_pair(j, *eg);
              });
              return nj;
            });
            return gen::container<std::vector<std::pair<std::unordered_map<std::string, double>, std::unordered_map<std::string, flom::Effector>>>>(gen::pair(j, e));
          }
        )));
  }
};

} // namespace rc

#endif
