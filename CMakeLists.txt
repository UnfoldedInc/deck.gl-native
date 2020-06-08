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

# Inspired by:
# https://github.com/uber/h3/blob/master/CMakeLists.txt
# https://github.com/google/s2geometry/blob/master/CMakeLists.txt
# https://dawn.googlesource.com/dawn/+/refs/heads/master/CMakeLists.txt

# gtest_discover_tests requires v3.12
# Currently the cmake version supported by Travis CI is 3.12.4
if (IOS)
    # We use the most recent version of cmake for iOS builds as even the minor
    # builds contain important patches
    cmake_minimum_required(VERSION 3.17.2)
else()
    cmake_minimum_required(VERSION 3.12)
endif()

set(DECK_VERSION 0.0.1)
set(DECK_SO_VERSION 1)
project(
    deck.gl
    DESCRIPTION "C++ renderer for deck.gl"
    HOMEPAGE_URL "https://github.com/UnfoldedInc/deck.gl-native"
    LANGUAGES C CXX
    VERSION ${DECK_VERSION}
    )

# Global configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Dependencies
include(CMakeDependentOption)

###########
# OPTIONS #
###########

# Note that treating warnings as errors can generate unexpected errors on 
# different compilers that aren't tested in our CI and is therefore by default OFF
option(DECK_WARNINGS_AS_ERRORS "Warnings are treated as errors" OFF)
option(DECK_ENABLE_COVERAGE "Creates coverage reports if enabled" OFF)
option(DECK_BUILD_EXAMPLES "Enables building examples" ON)
option(DECK_BUILD_TESTS "Enables building tests" ON)
option(DECK_ENABLE_GRAPHICS "When set to OFF, no graphics libraries will be included. Useful for running on CI" ON)
option(DECK_USE_FRAMEWORKS "Whether or not to bundle libraries as frameworks on macOS and iOS" ON)

# We're using clang-format-9 Linux package, which should have priority over any other version
find_program(CLANG_FORMAT_PATH NAMES clang-format-9 clang-format)
cmake_dependent_option(
    DECK_ENABLE_FORMAT "Enable running clang-format before compiling" ON
    "CLANG_FORMAT_PATH" OFF
    )

# If we're compiling for an Apple platform, use frameworks by default
cmake_dependent_option(
    DECK_USE_FRAMEWORKS "Whether or not to bundle libraries as frameworks on macOS and iOS" ON
    APPLE OFF
    )

# Default values for the backend-enabling options
set(ENABLE_D3D12 OFF)
set(ENABLE_METAL OFF)
set(ENABLE_OPENGL OFF)
set(ENABLE_VULKAN OFF)
if (WIN32 AND DECK_ENABLE_GRAPHICS)
    # TODO: Currently not supported
    # set(ENABLE_D3D12 ON)
    # set(ENABLE_VULKAN ON)
elseif(APPLE AND DECK_ENABLE_GRAPHICS)
    set(ENABLE_METAL ON)
elseif(UNIX AND DECK_ENABLE_GRAPHICS)
    set(ENABLE_OPENGL ON)
    # TODO: Dawn does not run Vulkan on Unix right now
    # set(ENABLE_VULKAN ON)
endif()

option(DECK_ENABLE_D3D12 "Enable compilation of the D3D12 backend" ${ENABLE_D3D12})
option(DECK_ENABLE_METAL "Enable compilation of the Metal backend" ${ENABLE_METAL})
option(DECK_ENABLE_OPENGL "Enable compilation of the OpenGL backend" ${ENABLE_OPENGL})
option(DECK_ENABLE_VULKAN "Enable compilation of the Vulkan backend" ${ENABLE_VULKAN})
option(DECK_ENABLE_NULL "Enable compilation of the Null backend" ON)

# If CMAKE_BUILD_TYPE is not specified explicitly by using -DCMAKE_BUILD_TYPE, default to Debug
if (NOT CMAKE_BUILD_TYPE)
    message(WARNING "CMAKE_BUILD_TYPE not set, forcing it to Debug")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

# Appends cmake folder to CMAKE_MODULE_PATH, used by Valgrind
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Project modules
set(DECK_MODULES probe.gl math.gl loaders.gl luma.gl deck.gl)
set(DECK_MODULE_PATH cpp/modules/)

# Configuration libraries
set(DECK_CONFIG_LIBRARY deckgl-internal-config)

# Global property that will be "topped-off" with example files
set_property(GLOBAL PROPERTY DECK_EXAMPLE_FILES "")

##################
# PLATFORM SETUP #
##################

# Determine which architecture/OS we're building for
# https://cmake.org/cmake/help/latest/variable/IOS.html
if (IOS)
    set(DECK_ARCH ${CMAKE_OSX_ARCHITECTURES}-ios)
elseif (APPLE)
    set(DECK_ARCH x64-osx)
elseif (UNIX)
    set(DECK_ARCH x64-linux)
else()
    message(FATAL_ERROR "Operating system currently not supported. Please consult the development team")
endif()

# All the supported platforms except iOS use GLFW
if (NOT IOS)
    set(LUMAGL_USES_GLFW TRUE)
endif()

# Path to folder with 3rd party dependencies
set(DECK_DEPS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cpp/deps/${DECK_ARCH}/lib)
# Path to supporting example data
set(DECK_DATA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data)

# Setup compile and link flags
set(DECK_COMPILE_FLAGS "")
set(DECK_LINK_FLAGS "")
if (NOT WIN32)
    # Enable all compiler warnings
    list(APPEND DECK_COMPILE_FLAGS -Wall)
    # Append flags useful for debugging if configuration is set to Debug
    list(APPEND DECK_COMPILE_FLAGS $<$<CONFIG:Debug>:-gdwarf-2 -g3 -O0 -fno-inline -fno-eliminate-unused-debug-types>)

    if (ENABLE_COVERAGE)
        list(APPEND DECK_COMPILE_FLAGS $<$<CONFIG:Debug>:--coverage>)
        # --coverage is not passed to the linker, so this option is needed to fully enable coverage.
        list(APPEND DECK_LINK_FLAGS $<$<CONFIG:Debug>:--coverage>)
    endif()

    if (WARNINGS_AS_ERRORS)
        list(APPEND DECK_COMPILE_FLAGS -Werror)
    endif()
endif()

###########################
# CONFIGURATION LIBRARIES #
###########################

add_library(${DECK_CONFIG_LIBRARY} INTERFACE)

target_include_directories(${DECK_CONFIG_LIBRARY} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/deps/${DECK_ARCH}/include
    )
target_compile_options(${DECK_CONFIG_LIBRARY} INTERFACE ${DECK_COMPILE_FLAGS})

# Add definitions based on options
if (DECK_ENABLE_GRAPHICS)
    target_compile_definitions(${DECK_CONFIG_LIBRARY} INTERFACE "LUMAGL_ENABLE_GRAPHICS")
endif()
if (DECK_ENABLE_D3D12)
    target_compile_definitions(${DECK_CONFIG_LIBRARY} INTERFACE "LUMAGL_ENABLE_BACKEND_D3D12")
    target_compile_definitions(${DECK_CONFIG_LIBRARY} INTERFACE "DAWN_ENABLE_BACKEND_D3D12")
endif()
if (DECK_ENABLE_METAL)
    target_compile_definitions(${DECK_CONFIG_LIBRARY} INTERFACE "LUMAGL_ENABLE_BACKEND_METAL")
    target_compile_definitions(${DECK_CONFIG_LIBRARY} INTERFACE "DAWN_ENABLE_BACKEND_METAL")
endif()
if (DECK_ENABLE_NULL)
    target_compile_definitions(${DECK_CONFIG_LIBRARY} INTERFACE "LUMAGL_ENABLE_BACKEND_NULL")
endif()
if (DECK_ENABLE_OPENGL)
    target_compile_definitions(${DECK_CONFIG_LIBRARY} INTERFACE "LUMAGL_ENABLE_BACKEND_OPENGL")
    target_compile_definitions(${DECK_CONFIG_LIBRARY} INTERFACE "DAWN_ENABLE_BACKEND_OPENGL")
endif()
if (DECK_ENABLE_VULKAN)
    target_compile_definitions(${DECK_CONFIG_LIBRARY} INTERFACE "LUMAGL_ENABLE_BACKEND_VULKAN")
    target_compile_definitions(${DECK_CONFIG_LIBRARY} INTERFACE "DAWN_ENABLE_BACKEND_VULKAN")
endif()
if (LUMAGL_USES_GLFW)
    target_compile_definitions(${DECK_CONFIG_LIBRARY} INTERFACE "LUMAGL_USES_GLFW")
endif()

##################
# BUNDLE LIBRARY #
##################

# Currently only used internally to link against for testing purposes
add_library(deckgl-bundle INTERFACE)

# Define module libs
foreach(DECK_MODULE ${DECK_MODULES})
    add_subdirectory(${DECK_MODULE_PATH}${DECK_MODULE})
endforeach()

target_link_libraries(deckgl-bundle INTERFACE ${DECK_MODULES})

############
# EXAMPLES #
############

set(DECK_EXAMPLES cpp/examples/deck.gl/ cpp/examples/luma.gl/)
if (DECK_BUILD_EXAMPLES)
    foreach(DECK_EXAMPLE ${DECK_EXAMPLES})
        add_subdirectory(${DECK_EXAMPLE})
    endforeach()
endif()

#########
# TESTS #
#########

if (DECK_BUILD_TESTS)
    # Depedencies
    include(CTest)
    include(GoogleTest)

    # Add 'deckgl-bundle-tests' executable
    add_executable(deckgl-bundle-tests)

    # Add test source files from all the modules
    foreach(DECK_MODULE ${DECK_MODULES})
        get_target_property(DECK_MODULE_TESTS ${DECK_MODULE} DECK_TEST_SOURCES)
        target_sources(deckgl-bundle-tests PRIVATE ${DECK_MODULE_TESTS})
    endforeach()

    target_sources(deckgl-bundle-tests PRIVATE cpp/tests/main.cc)
    
    # We're using pre-built dependencies from our dependency submodule
    # NO_DEFAULT_PATH and NO_CMAKE_FIND_ROOT_PATH make it so other lookup paths are ignored
    find_library(gtest_LIB gtest PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
    target_link_libraries(deckgl-bundle-tests PRIVATE ${gtest_LIB} deckgl-bundle)

    # Extract registered Google Tests from the executable and add them to ctest
    # https://cmake.org/cmake/help/latest/module/GoogleTest.html
    gtest_discover_tests(deckgl-bundle-tests)
endif()

############
# COVERAGE #
############

if (DECK_ENABLE_COVERAGE)
    # Add a "coverage" target that generates a coverage report.
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/scripts/coverage.sh"
         INPUT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/coverage.sh.in")
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "coverage.info")
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "coverage.cleaned.info")
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "coverage")
    add_custom_target(coverage
        COMMAND bash "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/scripts/coverage.sh" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}")
    add_custom_target(clean-coverage
        # Before running coverage, clear all counters
        COMMAND lcov --rc lcov_branch_coverage=1 --directory '${CMAKE_CURRENT_BINARY_DIR}' --zerocounters
        COMMENT "Zeroing counters"
        )
endif()

##############
# FORMATTING #
##############

if (DECK_ENABLE_FORMAT)
    # Gather a list of source files that will be passed onto the formatter
    foreach(DECK_MODULE ${DECK_MODULES})
        get_target_property(DECK_MODULE_SOURCES ${DECK_MODULE} DECK_ALL_SOURCES)
        list(APPEND DECK_SOURCE_LIST ${DECK_MODULE_SOURCES})
    endforeach()
    get_property(DECK_EXAMPLE_FILES GLOBAL PROPERTY DECK_EXAMPLE_FILES)
    list(APPEND DECK_SOURCE_LIST ${DECK_EXAMPLE_FILES})

    # Format
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_PATH}
        -style=file
        -i
        ${DECK_SOURCE_LIST}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Formatting sources"
        )
    # Always do formatting
    add_dependencies(deckgl-bundle format)
elseif (NOT CLANG_FORMAT_PATH)
    message(WARNING "clang-format was not detected, so automatic source code reformatting is disabled")
endif()
