#!/bin/bash

# Note when switching compiler: `rm -fr build`

cmake -S . -B build/cpplint -DCMAKE_C_COMPILER=gcc-9 -DCMAKE_CXX_COMPILER=g++-9 -DCMAKE_CXX_CPPLINT=cpplint
cmake --build build/cpplint -j 16
build/clang/deckgl-bundle-tests
