![flom](docs/logo.svg)

[![Travis CI](https://img.shields.io/travis/MonoMotion/flom.svg?style=flat-square)](https://travis-ci.org/MonoMotion/flom)
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2FDeepL2%2Fflom.svg?type=small)](https://app.fossa.io/projects/git%2Bgithub.com%2FDeepL2%2Fflom?ref=badge_small)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/37de359208f14aa5ab2e2a67b170335a)](https://www.codacy.com/app/coord-e/flom?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=MonoMotion/flom&amp;utm_campaign=Badge_Grade)
[![license](https://img.shields.io/github/license/MonoMotion/flom.svg?style=flat-square)](COPYING)
[![GitHub release](https://img.shields.io/github/release/MonoMotion/flom.svg?style=flat-square)](https://github.com/MonoMotion/flom/releases)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-brightgreen.svg?style=flat-square)](https://isocpp.org/std/status)

Motion data exchange format

## Installation

Download and install suitable package from [releases](https://github.com/MonoMotion/flom/releases)

Latest build artifacts can be found at [![bintray](https://img.shields.io/bintray/v/monomotion/flom/latest.svg?style=flat-square)](https://dl.bintray.com/monomotion/flom/)

Or you can [build manually](#build-manually)

## Build manually

### Build Requirements

- Boost (headers)
- protobuf 3.0.0 or later
- cmake 3.9.6 or later
- C++17 compiler
  - clang 5.0 or later
  - gcc 6.1 or later
- C++17 standard library
  - libc++ 7 or later
  - libstdc++ 6 or later

### How to build

```shell
git clone https://github.com/MonoMotion/flom --recursive
cd flom
mkdir build && cd $_
cmake ..
make -j $(nproc)
sudo make install
```

## License
[![FOSSA Status](https://app.fossa.io/api/projects/git%2Bgithub.com%2FDeepL2%2Fflom.svg?type=large)](https://app.fossa.io/projects/git%2Bgithub.com%2FDeepL2%2Fflom?ref=badge_large)
