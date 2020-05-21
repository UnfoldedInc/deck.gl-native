#!/bin/bash

# Note when switching compiler: `rm -fr build`

cmake -S . -B build/gcc -DCMAKE_C_COMPILER=gcc-9 -DCMAKE_CXX_COMPILER=g++-9
cmake --build build/gcc -j 16
build/clang/deckgl-bundle-tests
