# Copyright (c) 2020 Unfolded Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
project(examples LANGUAGES C CXX)

set(DAWN_DIR "../../../../dawn")
set(VCPKG_ROOT "../../../../vcpkg")

include_directories(SYSTEM ${DAWN_DIR}/src/include)
# TODO: Should target Release directory for Release config
include_directories(SYSTEM ${DAWN_DIR}/out/Debug/gen/src/include)
include_directories(${DAWN_DIR}/src)
include_directories(${DAWN_DIR}/third_party/glm)

# TODO: Add actual configuration options
add_definitions(-DDAWN_ENABLE_BACKEND_METAL)

# TODO: Should target Release directory for Release config
link_directories(${DAWN_DIR}/out ${DAWN_DIR}/out/Debug ${DAWN_DIR}/out/Debug/obj ${DAWN_DIR}/out/Debug/obj/src/common)
link_directories(${DAWN_DIR}/src/dawn ${DAWN_DIR}/src/common)

include_directories(${VCPKG_ROOT}/installed/x64-osx/include)
link_directories(${VCPKG_ROOT}/installed/x64-osx/lib)

add_library(dawn_sample_utils STATIC)
target_sources(dawn_sample_utils PRIVATE
    "SampleUtils.cpp"
    "SampleUtils.h"
)
target_link_libraries(dawn_sample_utils PUBLIC
    shaderc
    ${DAWN_DIR}/out/Debug/obj/src/dawn/dawncpp/webgpu_cpp.o
    dawn_proc
    common
    dawn_native
    dawn_wire
    dawn_utils
    dawn_bindings
    glfw3
)

# When building for Mac, and Metal is enabled, we need to link against system frameworks
# TODO: Should be able to use find_package for these, it does not seem to work for some reason
target_link_libraries(dawn_sample_utils PUBLIC
    "-framework Metal"
    "-framework QuartzCore"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework IOSurface"
    "-framework QuartzCore"
)
set_target_properties(dawn_sample_utils PROPERTIES LINK_FLAGS "-Wl,-F/System/Library/Frameworks")

# add_executable(DawnAnimometer "Dawn_Animometer")
# target_link_libraries(DawnAnimometer dawn_sample_utils)
