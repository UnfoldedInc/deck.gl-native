#!/bin/bash

# Note when switching compiler: `rm -fr build`

mkdir -p build/cpplint
cd build/cpplint
cmake "-DCMAKE_CXX_CPPLINT=cpplint" ../..
make -j 16
ctest --output-on-failure
# make test #  same output as `ctest`?
