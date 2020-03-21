#!/bin/bash

# Note when switching compiler: `rm -fr build`

mkdir -p build/cpplint
cd build/cpplint
cmake -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake "-DCMAKE_CXX_CPPLINT=cpplint" ../..
make -j
ctest --output-on-failure
# make test #  same output as `ctest`?
