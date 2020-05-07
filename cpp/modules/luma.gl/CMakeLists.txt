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
    webgpu/src/backends/backend-binding.h
    )
set(WEBGPU_SOURCE_FILE_LIST
    webgpu/src/combo-render-pipeline-descriptor.cpp
    webgpu/src/shaderc-utils.cpp
    webgpu/src/webgpu-utils.cpp
    webgpu/src/webgpu-helpers.cpp
    webgpu/src/backends/backend-binding.cpp
    )
set(WEBGPU_TESTS_SOURCE_FILE_LIST
    webgpu/test/webgpu-test.cpp
    )

set(GARROW_HEADER_FILE_LIST
    garrow.h
    garrow/src/key-value-metadata.h
    garrow/src/field.h
    garrow/src/schema.h
    garrow/src/array.h
    garrow/src/table.h
    garrow/src/util/arrow-utils.h
    garrow/src/util/webgpu-utils.h
    )
set(GARROW_SOURCE_FILE_LIST
    garrow/src/key-value-metadata.cc
    garrow/src/schema.cc
    garrow/src/array.cc
    garrow/src/table.cc
    garrow/src/util/arrow-utils.cc
    garrow/src/util/webgpu-utils.cc
    )
set(GARROW_TESTS_SOURCE_FILE_LIST
    garrow/test/util/arrow-utils-test.cc
    )

set(HEADER_FILE_LIST
    core.h
    core/src/animation-loop.h
    core/src/glfw-animation-loop.h
    core/src/model.h
    core/src/blit-model.h
    core/src/size.h
    )
set(SOURCE_FILE_LIST
    core/src/animation-loop.cpp
    core/src/glfw-animation-loop.cpp
    core/src/model.cpp
    core/src/blit-model.cc
    )
set(TESTS_SOURCE_FILE_LIST
    )

find_library(dawn_proc_LIB dawn_proc PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH)
find_library(dawn_native_LIB dawn_native PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH)
find_library(glfw_LIB glfw PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH)
find_library(shaderc_LIB shaderc_combined PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH)
find_library(arrow_LIB arrow PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH)

target_link_libraries(luma.gl PUBLIC ${DECK_LINK_FLAGS}
    ${dawn_proc_LIB}
    ${dawn_native_LIB}
    ${glfw_LIB}
    ${shaderc_LIB}
    ${arrow_LIB}
    ${DECK_DEPS_PATH}/webgpu_cpp.o
    )
target_link_libraries(luma.gl PUBLIC ${DECK_CONFIG_LIBRARY} math.gl probe.gl)
if (NOT APPLE)
    # TODO: Check if still needed
    target_link_libraries(luma.gl PUBLIC -pthread -ldl -lX11)
endif()

# GLFW dependencies
if (APPLE)
    find_library(Cocoa_LIB Cocoa)
    find_library(IOKit_LIB IOKit)
    find_library(CoreFoundation_LIB CoreFoundation)
    find_library(CoreVideo_LIB CoreVideo)
    target_link_libraries(luma.gl PUBLIC ${DECK_LINK_FLAGS}
        ${Cocoa_LIB}
        ${IOKit_LIB}
        ${CoreFoundation_LIB}
        ${CoreVideo_LIB}
        )
endif()

# Backend specific files and libraries
if (DECK_ENABLE_D3D12)
    list(APPEND WEBGPU_SOURCE_FILE_LIST webgpu/src/backends/d3d12-binding.cpp)
    # TODO: Currently not supported. Should link against D3D12 libs
endif()
if (DECK_ENABLE_METAL)
    list(APPEND WEBGPU_SOURCE_FILE_LIST webgpu/src/backends/metal-binding.mm)

    # macOS frameworks related to Metal
    find_library(Metal_LIB Metal)
    find_library(QuartzCore_LIB QuartzCore)
    find_library(IOKit_LIB IOKit)
    find_library(IOSurface_LIB IOSurface)
    target_link_libraries(luma.gl PUBLIC ${DECK_LINK_FLAGS}
        ${Metal_LIB}
        ${QuartzCore_LIB}
        ${IOKit_LIB}
        ${IOSurface_LIB}
        )
endif()
if (DECK_ENABLE_NULL)
    list(APPEND WEBGPU_SOURCE_FILE_LIST webgpu/src/backends/null-binding.cpp)
endif()
if (DECK_ENABLE_OPENGL)
    list(APPEND WEBGPU_SOURCE_FILE_LIST webgpu/src/backends/opengl-binding.cpp)
endif()
if (DECK_ENABLE_VULKAN)
    list(APPEND WEBGPU_SOURCE_FILE_LIST webgpu/src/backends/vulkan-binding.cpp)
endif()

# Specify sources that'll be compiled
target_sources(luma.gl PRIVATE ${SOURCE_FILE_LIST} ${WEBGPU_SOURCE_FILE_LIST} ${GARROW_SOURCE_FILE_LIST})

# Export source list of this module
# NOTE: We transform relative paths to absolute paths before exporting
set(MODULE_SOURCE_FILES
    ${WEBGPU_HEADER_FILE_LIST} ${WEBGPU_SOURCE_FILE_LIST}
    ${GARROW_HEADER_FILE_LIST} ${GARROW_SOURCE_FILE_LIST}
    ${HEADER_FILE_LIST} ${SOURCE_FILE_LIST}
    ${WEBGPU_TESTS_SOURCE_FILE_LIST} ${GARROW_TESTS_SOURCE_FILE_LIST} ${TESTS_SOURCE_FILE_LIST}
    )
list(TRANSFORM MODULE_SOURCE_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
set_property(TARGET luma.gl PROPERTY DECK_ALL_SOURCES ${MODULE_SOURCE_FILES})

set(MODULE_TEST_FILES ${TESTS_SOURCE_FILE_LIST} ${WEBGPU_TESTS_SOURCE_FILE_LIST} ${GARROW_TESTS_SOURCE_FILE_LIST})
list(TRANSFORM MODULE_TEST_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
set_property(TARGET luma.gl PROPERTY DECK_TEST_SOURCES ${MODULE_TEST_FILES})
