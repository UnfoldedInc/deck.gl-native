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

add_library(luma.gl)

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

target_link_libraries(luma.gl PUBLIC
    ${CMAKE_SOURCE_DIR}/cpp/deps/${DAWN_ARCH}/lib/webgpu_cpp.o
    dawn_proc
    common
    dawn_native
    dawn_wire
    dawn_utils
    dawn_bindings
    shaderc
    glfw
    )

# Specify sources that'll be compiled
target_sources(luma.gl PRIVATE ${SOURCE_FILE_LIST})

# Export source list of this module
set_property(TARGET luma.gl PROPERTY DECK_ALL_SOURCES 
    ${WEBGPU_HEADER_FILE_LIST} ${WEBGPU_SOURCE_FILE_LIST}
    ${HEADER_FILE_LIST} ${SOURCE_FILE_LIST}
    ${WEBGPU_TESTS_SOURCE_FILE_LIST} ${TESTS_SOURCE_FILE_LIST}
    )
set_property(TARGET luma.gl PROPERTY DECK_TEST_SOURCES ${TESTS_SOURCE_FILE_LIST} ${WEBGPU_TESTS_SOURCE_FILE_LIST})
