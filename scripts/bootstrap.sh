#/bin/bash

VCPKG_ROOT=${VCPKG_ROOT:-../vcpkg}
if [ -d "${VCPKG_ROOT}" ]; then
  "${VCPKG_ROOT}/vcpkg" install jsoncpp
  "${VCPKG_ROOT}/vcpkg" install gtest
  "${VCPKG_ROOT}/vcpkg" install range-v3
else
  echo "vcpkg not found, skipping (you might need to set VCPKG_ROOT)"
fi
