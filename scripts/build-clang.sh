#!/bin/bash

VCPKG_ROOT=${VCPKG_ROOT:-../vcpkg}

# Note when switching compiler: `rm -fr build`

mkdir -p build/clang
cd build/clang
cmake "-DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ../..
make -j 16
ctest --output-on-failure
# make test #  same output as `ctest`?
