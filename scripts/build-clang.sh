#!/bin/bash

# Note when switching compiler: `rm -fr build`

cmake -S . -B build/clang -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build/clang -j 16
build/clang/deckgl-bundle-tests
