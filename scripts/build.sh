#!/bin/bash

cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake ..
make
ctest
# make test #  same output as `ctest`?
