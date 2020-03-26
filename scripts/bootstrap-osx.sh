#/bin/bash

VCPKG_ROOT=${VCPKG_ROOT:-../vcpkg}

## Platform independent bootstrap (mainly vcpkg installs)
./scripts/bootstrap.sh

## Graphics dependencies for luma.gl
"${VCPKG_ROOT}/vcpkg" install glfw3
"${VCPKG_ROOT}/vcpkg" install shaderc # TODO(ib): compiling shaderc from vcpkg is very slow!

## Install loaders.gl dependencies
brew update
brew install gdal
