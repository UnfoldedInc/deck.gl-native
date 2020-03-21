#!/bin/bash

# Note when switching compiler: `rm -fr build`

mkdir -p build/clang
cd build/clang
cmake -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake ../..
make -j
ctest --output-on-failure
# make test #  same output as `ctest`?
