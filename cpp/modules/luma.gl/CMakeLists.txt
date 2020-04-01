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

add_library(lumagl)

# File lists that are maintained manually
set(WEBGPU_HEADER_FILE_LIST
    webgpu.h
    webgpu/src/combo-render-pipeline-descriptor.h
    webgpu/src/shaderc-utils.h
    webgpu/src/swap-chain-utils.h
    webgpu/src/webgpu-constants.h
    webgpu/src/webgpu-utils.h
    webgpu/src/webgpu-helpers.h
    webgpu/src/terrible-command-buffer.h
    webgpu/src/backends/backend-binding.h
    )
set(WEBGPU_SOURCE_FILE_LIST
    webgpu/src/combo-render-pipeline-descriptor.cpp
    webgpu/src/shaderc-utils.cpp
    webgpu/src/terrible-command-buffer.cpp
    webgpu/src/webgpu-utils.cpp
    webgpu/src/webgpu-helpers.cpp
    webgpu/src/backends/backend-binding.cpp
    # TODO: These should be included on per-platform basis. See src/utils/CMakeLists.txt in dawn repo
    webgpu/src/backends/metal-binding.mm
    #webgpu/src/backends/vulkan-binding.cpp
    #webgpu/src/backends/d3d12-binding.cpp
    #webgpu/src/backends/null-binding.cpp
    #webgpu/src/backends/opengl-binding.cpp
    )
set(WEBGPU_TESTS_SOURCE_FILE_LIST
    webgpu/test/webgpu-test.cpp
    )

set(HEADER_FILE_LIST
    core.h
    core/src/animation-loop.h
    core/src/glfw-animation-loop.h
    core/src/model.h
    )
set(SOURCE_FILE_LIST
    core/src/animation-loop.cpp
    # core/src/glfw-animation-loop.cpp
    core/src/model.cpp
    )
set(TESTS_SOURCE_FILE_LIST
    )

# Some of the include paths are relative to modules directory
target_include_directories(lumagl PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/cpp/modules/>)

# Export source list of this module
set_property(TARGET lumagl PROPERTY DECK_ALL_SOURCES 
    ${WEBGPU_HEADER_FILE_LIST} ${WEBGPU_SOURCE_FILE_LIST}
    ${HEADER_FILE_LIST} ${SOURCE_FILE_LIST}
    ${WEBGPU_TESTS_SOURCE_FILE_LIST} ${TESTS_SOURCE_FILE_LIST}
    )
set_property(TARGET lumagl PROPERTY DECK_TEST_SOURCES ${TESTS_SOURCE_FILE_LIST} ${WEBGPU_TESTS_SOURCE_FILE_LIST})

# Dependencies
find_package(glfw3 CONFIG REQUIRED)    # Windowing integration for dawn

# TODO: Revisit once we start making use of the dependency repo
set(DAWN_DIR "${CMAKE_SOURCE_DIR}/../dawn")
target_include_directories(lumagl PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/cpp/deps/dawn/include/>)
target_link_libraries(lumagl PUBLIC
    ${DAWN_DIR}/out/Debug/obj/src/dawn/dawncpp/webgpu_cpp.o
    ${DAWN_DIR}/out/Debug/obj/src/dawn/libdawn_proc.a
    ${DAWN_DIR}/out/Debug/obj/src/common/libcommon.a
    ${DAWN_DIR}/out/Debug/obj/libdawn_native.a
    ${DAWN_DIR}/out/Debug/obj/libdawn_wire.a
    ${DAWN_DIR}/out/Debug/obj/libdawn_utils.a
    ${DAWN_DIR}/out/Debug/obj/libdawn_bindings.a
    glfw
    )

# Specify sources that'll be compiled
target_sources(lumagl PRIVATE ${SOURCE_FILE_LIST})
