#!/bin/bash

# Note when switching compiler: `rm -fr build`

mkdir -p build/gcc
cd build/gcc
cmake -DCMAKE_C_COMPILER=gcc-9 -DCMAKE_CXX_COMPILER=g++-9 ../..
make -j 16
ctest --output-on-failure
# make test #  same output as `ctest`?
