#!/bin/bash

# Note when switching compiler: `rm -fr build`

mkdir -p build/gcc
cd build/gcc
VCPKG_ROOT=${VCPKG_ROOT:-../vcpkg}
cmake "-DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" -DCMAKE_C_COMPILER=gcc-9 -DCMAKE_CXX_COMPILER=g++-9 ../..
make -j
ctest --output-on-failure
# make test #  same output as `ctest`?
