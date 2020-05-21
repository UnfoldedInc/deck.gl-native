#!/bin/bash

set -e

# The following are paired together in order to form a build target
declare -a archs=("arm64" "arm64e" "x86_64")
declare -a sysroots=("iphoneos" "iphoneos" "iphonesimulator")
declare -a deployment_targets=("12.0" "12.0" "13.0")

install_dir="`pwd`/install"

# Build for each architecture individually
for i in $(seq 0 $((${#archs[@]}-1))) # {0..${#archs[@]}}
do
  cmake -S . -B build/${archs[$i]} -GXcode -DCMAKE_INSTALL_PREFIX=$install_dir/${archs[$i]} -DDECK_BUILD_EXAMPLES=OFF -DDECK_BUILD_TESTS=OFF -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_ARCHITECTURES=${archs[$i]} -DCMAKE_OSX_SYSROOT=${sysroots[$i]} -DCMAKE_OSX_DEPLOYMENT_TARGET=${deployment_targets[$i]} -DCMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED=NO -DCMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE=NO
  cmake --build build/${archs[$i]} --config Debug --target install -j 16
done

# Copy over a set of frameworks we'll use as a base
mkdir -p install/combined/lib
cp -r install/${archs[0]}/lib/* install/combined/lib

## Declare the modules we're building, and iterate over them to combine the framework architectures
declare -a modules=("deck.gl" "luma.gl" "loaders.gl" "math.gl" "probe.gl")
for module in ${modules[@]}
do
  command="lipo -create "
  for arch in ${archs[@]}
  do
    command="$command $install_dir/$arch/lib/$module.framework/$module "
  done
  command="$command -output $install_dir/combined/lib/$module.framework/$module"
  eval $command
done
