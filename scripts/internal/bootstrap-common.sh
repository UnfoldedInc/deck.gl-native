#/bin/bash

VCPKG_ROOT=${VCPKG_ROOT:-../vcpkg}

## General C++ Dependencies
if [ "$SKIP_VCPKG" != "true" ] ; then
  "${VCPKG_ROOT}/vcpkg" install jsoncpp
  "${VCPKG_ROOT}/vcpkg" install gtest
  "${VCPKG_ROOT}/vcpkg" install range-v3
  "${VCPKG_ROOT}/vcpkg" install fmt
fi
