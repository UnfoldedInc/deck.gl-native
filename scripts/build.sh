#!/bin/bash

cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake ..
make
<<<<<<< HEAD
ctest # --verbose
=======
ctest --output-on-failure
>>>>>>> json: Hydrate Prop objects from JSON, step 1
# make test #  same output as `ctest`?
