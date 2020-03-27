#/bin/bash

#
# Clone deck.gl-native itself
#
git clone git@github.com:UnfoldedInc/deck.gl-native.git

#
# Clone and configure vcpkg
#
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh -disableMetrics # Compile
./vcpkg integrate install # Make packages globally availble

#
# Clone and configure depot_tools (Needed for building dawn)
#
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
echo 'export PATH=${PATH}:$PWD/depot_tools' >>~/.bash_profile

#
# Clone and build the dawn repo
#
git clone https://dawn.googlesource.com/dawn dawn
cd dawn

# Bootstrap the gclient configuration
cp scripts/standalone.gclient .gclient

# Fetch external dependencies and toolchains with gclient
gclient sync

#
# Build dawn
# https://dawn.googlesource.com/dawn/+/HEAD/docs/buiding.md
#

# Configure build
echo Add `is_debug=true` in the editor and save
gn args out/Debug

# Build
ninja -C out/Debug

# Run dawn tests
./out/Debug/dawn_end2end_tests
