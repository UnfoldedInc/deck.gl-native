#/bin/bash

VCPKG_ROOT=${VCPKG_ROOT:-../vcpkg}

## General C++ Dependencies
"${VCPKG_ROOT}/vcpkg" install jsoncpp
"${VCPKG_ROOT}/vcpkg" install gtest
"${VCPKG_ROOT}/vcpkg" install range-v3
"${VCPKG_ROOT}/vcpkg" install fmt