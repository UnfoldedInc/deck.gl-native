#!/bin/bash

# Note when switching compiler: `rm -fr build`

mkdir -p build/cpplint
cd build/cpplint
VCPKG_ROOT=${VCPKG_ROOT:-../vcpkg}
cmake "-DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" "-DCMAKE_CXX_CPPLINT=cpplint" ../..
make -j
ctest --output-on-failure
# make test #  same output as `ctest`?
