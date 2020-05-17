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
set_target_properties(luma.gl PROPERTIES VERSION ${DECK_VERSION})
set_target_properties(luma.gl PROPERTIES SOVERSION ${DECK_SO_VERSION})

# File lists that are maintained manually
set(WEBGPU_HEADER_FILE_LIST
    webgpu.h
    webgpu/src/combo-render-pipeline-descriptor.h
    webgpu/src/shaderc-utils.h
    webgpu/src/swap-chain-utils.h
    webgpu/src/webgpu-constants.h
    webgpu/src/webgpu-utils.h
    webgpu/src/webgpu-helpers.h
    )
set(WEBGPU_SOURCE_FILE_LIST
    webgpu/src/combo-render-pipeline-descriptor.cpp
    webgpu/src/shaderc-utils.cpp
    webgpu/src/webgpu-utils.cpp
    webgpu/src/webgpu-helpers.cpp
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

set(CORE_HEADER_FILE_LIST
    core.h
    core/src/animation-loop.h
    core/src/model.h
    core/src/blit-model.h
    core/src/size.h
    )
set(CORE_SOURCE_FILE_LIST
    core/src/animation-loop.cpp
    core/src/model.cpp
    core/src/blit-model.cc
    )
set(CORE_TESTS_SOURCE_FILE_LIST
    )

# We're using pre-built dependencies from our dependency submodule
# NO_DEFAULT_PATH and NO_CMAKE_FIND_ROOT_PATH make it so other lookup paths are ignored
find_library(dawn_proc_LIB dawn_proc PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
find_library(dawn_native_LIB dawn_native PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
find_library(arrow_LIB arrow PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
find_library(shaderc_LIB shaderc_combined PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

target_link_libraries(luma.gl PUBLIC ${DECK_LINK_FLAGS}
    ${dawn_proc_LIB}
    ${dawn_native_LIB}
    ${arrow_LIB}
    ${shaderc_LIB}
    ${DECK_DEPS_PATH}/webgpu_cpp.o
    )
target_link_libraries(luma.gl PUBLIC ${DECK_CONFIG_LIBRARY} math.gl probe.gl)

# All the supported platforms except iOS use GLFW
if (NOT IOS)
    set(LUMAGL_USES_GLFW TRUE)
endif()

if (LUMAGL_USES_GLFW)
    list(APPEND WEBGPU_HEADER_FILE_LIST webgpu/src/backends/backend-binding.h)
    list(APPEND WEBGPU_SOURCE_FILE_LIST webgpu/src/backends/backend-binding.cpp)

    list(APPEND CORE_HEADER_FILE_LIST core/src/glfw-animation-loop.h)
    list(APPEND CORE_SOURCE_FILE_LIST core/src/glfw-animation-loop.cpp)

    find_library(glfw_LIB glfw PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH)
    target_link_libraries(luma.gl PUBLIC ${DECK_CONFIG_LIBRARY} ${glfw_LIB})

    # macOS requires additional frameworks to be linked
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
endif()

# Additional Linux setup
if (UNIX AND NOT APPLE)
    # TODO(isaac@unfolded.ai): Check if still needed
    target_link_libraries(luma.gl PUBLIC -pthread -ldl -lX11)
endif()

# Backend specific files and libraries
if (DECK_ENABLE_D3D12)
    if (LUMAGL_USES_GLFW)
        list(APPEND WEBGPU_SOURCE_FILE_LIST webgpu/src/backends/d3d12-binding.cpp)
    endif()

    # TODO: Currently not supported. Should link against D3D12 libs
endif()
if (DECK_ENABLE_METAL)
    if (LUMAGL_USES_GLFW)
        list(APPEND WEBGPU_SOURCE_FILE_LIST webgpu/src/backends/metal-binding.mm)
    endif()

    # Frameworks related to Metal
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
    if (LUMAGL_USES_GLFW)
        list(APPEND WEBGPU_SOURCE_FILE_LIST webgpu/src/backends/null-binding.cpp)
    endif()
endif()
if (DECK_ENABLE_OPENGL)
    if (LUMAGL_USES_GLFW)
        list(APPEND WEBGPU_SOURCE_FILE_LIST webgpu/src/backends/opengl-binding.cpp)
    endif()
endif()
if (DECK_ENABLE_VULKAN)
    if (LUMAGL_USES_GLFW)
        list(APPEND WEBGPU_SOURCE_FILE_LIST webgpu/src/backends/vulkan-binding.cpp)
    endif()
endif()

set(HEADER_FILE_LIST ${WEBGPU_HEADER_FILE_LIST} ${GARROW_HEADER_FILE_LIST} ${CORE_HEADER_FILE_LIST})
set(SOURCE_FILE_LIST ${WEBGPU_SOURCE_FILE_LIST} ${GARROW_SOURCE_FILE_LIST} ${CORE_SOURCE_FILE_LIST})
set(TESTS_SOURCE_FILE_LIST ${WEBGPU_TESTS_SOURCE_FILE_LIST} ${GARROW_TESTS_SOURCE_FILE_LIST} ${CORE_TESTS_SOURCE_FILE_LIST})

# Specify sources that'll be compiled
target_sources(luma.gl PRIVATE ${SOURCE_FILE_LIST})

# Export source list of this module
# NOTE: We transform relative paths to absolute paths before exporting
set(MODULE_SOURCE_FILES
    ${HEADER_FILE_LIST}
    ${SOURCE_FILE_LIST}
    ${TESTS_SOURCE_FILE_LIST}
    )
list(TRANSFORM MODULE_SOURCE_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
set_property(TARGET luma.gl PROPERTY DECK_ALL_SOURCES ${MODULE_SOURCE_FILES})

set(MODULE_TEST_FILES ${TESTS_SOURCE_FILE_LIST})
list(TRANSFORM MODULE_TEST_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
set_property(TARGET luma.gl PROPERTY DECK_TEST_SOURCES ${MODULE_TEST_FILES})

# Installation
install(TARGETS luma.gl
    LIBRARY
        DESTINATION lib
        COMPONENT Libraries
    ARCHIVE
        DESTINATION lib
        COMPONENT Libraries
    )

# In order to maintain the hierarchy within this module, we install the files
# one by one into appropriate directories
foreach (HEADER_FILE ${HEADER_FILE_LIST})
    get_filename_component(HEADER_DIRECTORY ${HEADER_FILE} DIRECTORY)
    # If we're using frameworks, there's no need for install directory as frameworks contain headers
    if (NOT DECK_USE_FRAMEWORKS)
        install(FILES ${HEADER_FILE} 
            DESTINATION include/luma.gl/${HEADER_DIRECTORY}
            COMPONENT Development
            )
    else()
        # Provide a prefix directory so that headers are not flattened
        set_source_files_properties(${HEADER_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Headers/${HEADER_DIRECTORY})
    endif()
endforeach()

if (DECK_USE_FRAMEWORKS)
    # Specyfing PUBLIC_HEADER in set_target_properties currently flattens the hierarchy which we do not want
    # The workaround is to specify these as sources in combination with prefix addition above
    # https://gitlab.kitware.com/cmake/cmake/issues/16739
    target_sources(luma.gl PRIVATE ${HEADER_FILE_LIST})

    set_target_properties(luma.gl PROPERTIES
        FRAMEWORK ${DECK_USE_FRAMEWORKS}
        FRAMEWORK_VERSION ${DECK_VERSION}
        MACOSX_FRAMEWORK_IDENTIFIER ai.unfolded.lumagl
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${DECK_VERSION}
        )
endif()
