#/bin/bash
VCPKG_ROOT=${VCPKG_ROOT:-../vcpkg}

## Platform independent bootstrap (mainly vcpkg installs)
./scripts/internal/bootstrap-common.sh

## Graphics dependencies for luma.gl
"${VCPKG_ROOT}/vcpkg" install glfw3
"${VCPKG_ROOT}/vcpkg" install shaderc # TODO(ib@unfolded.ai): compiling shaderc from vcpkg is very slow!

## Install loaders.gl dependencies
brew install gdal

# Install build tools
brew update
brew install gcc cmake clang-format lcov
