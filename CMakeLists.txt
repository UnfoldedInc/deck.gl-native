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

# Inspired by https://github.com/uber/h3/blob/master/CMakeLists.txt
# and by https://github.com/google/s2geometry/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 3.12)

project(
    deck.gl
    DESCRIPTION "C++ renderer for deck.gl"
    HOMEPAGE_URL "https://github.com/UnfoldedInc/deck.gl-native"
    LANGUAGES C CXX
    )

# Dependencies
include(CMakeDependentOption)

# Global configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# If CMAKE_BUILD_TYPE is not specified explicitly by using -DCMAKE_BUILD_TYPE, default to Debug
if (NOT CMAKE_BUILD_TYPE)
    message(WARNING "CMAKE_BUILD_TYPE not set, forcing it to Debug")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

# Appends cmake folder to CMAKE_MODULE_PATH, used by Valgrind
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Options

# Not a good idea to leave this on because it will generate errors on different compilers
# that aren't tested in our CI. Can be useful for developers in order to keep code clean
# of warnings.
option(DECK_WARNINGS_AS_ERRORS "Warnings are treated as errors" OFF)
option(DECK_ENABLE_COVERAGE "Creates coverage reports if enabled" OFF)
option(DECK_BUILD_EXAMPLES "Enables building examples" ON)
option(DECK_BUILD_TESTS "Enables building tests" ON)

find_program(CLANG_FORMAT_PATH clang-format)
cmake_dependent_option(
    DECK_ENABLE_FORMAT "Enable running clang-format before compiling" ON
    "CLANG_FORMAT_PATH" OFF)

# Project modules
# NOTE: Trying to emulate a map here, order and length of these matters
set(DECK_MODULES
    probegl
    mathgl
    loadersgl
    lumagl
    deckgl
    )
set(DECK_MODULE_PATHS
    cpp/modules/probe.gl/
    cpp/modules/math.gl/
    cpp/modules/loaders.gl/
    cpp/modules/luma.gl/
    cpp/modules/deck.gl/
    )
list(LENGTH DECK_MODULES DECK_MODULE_COUNT)
math(EXPR DECK_MODULE_COUNT "${DECK_MODULE_COUNT}-1")

# Setup compile and link flags
set(DECK_COMPILE_FLAGS "")
set(DECK_LINK_FLAGS "")
if (NOT WIN32)
    # Enable all compiler warnings
    list(APPEND DECK_COMPILE_FLAGS -Wall)
    # Append flags useful for debugging if configuration is set to Debug
    list(APPEND DECK_COMPILE_FLAGS $<$<CONFIG:Debug>:-gdwarf-2 -g3 -O0 -fno-inline -fno-eliminate-unused-debug-types>)

    if(ENABLE_COVERAGE)
        list(APPEND DECK_COMPILE_FLAGS $<$<CONFIG:Debug>:--coverage>)
        # --coverage is not passed to the linker, so this option is needed to fully enable coverage.
        list(APPEND DECK_LINK_FLAGS $<$<CONFIG:Debug>:--coverage>)
    endif()

    if(WARNINGS_AS_ERRORS)
        list(APPEND DECK_COMPILE_FLAGS -Werror)
    endif()
endif()

# Set these globally, for this folder and all the subfolders
# NOTE: Quotes add ; separators in between list elements, which is what add_compile_options needs
add_compile_options("${DECK_COMPILE_FLAGS}")
add_link_options(${DECK_LINK_FLAGS})

# TODO: Clean up once we switch to dependency repo
# TODO: Workaround for using Homebrew due to not being able
# to (efficiently or otherwise) install certain modules through vcpkg
include_directories(/usr/local/Cellar/apache-arrow/0.16.0/include)
link_directories(/usr/local/Cellar/apache-arrow/0.16.0/lib)

# TODO: Defining the same path on a target doesn't work...
include_directories($<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/cpp/deps/dawn/include/>)
include_directories($<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/../vcpkg/installed/x64-osx/include/>)

##################
# BUNDLE LIBRARY #
##################

add_library(deckgl-bundle INTERFACE)

# Define module libs
foreach(i RANGE ${DECK_MODULE_COUNT})
    list(GET DECK_MODULE_PATHS ${i} DECK_MODULE_PATH)
    add_subdirectory(${DECK_MODULE_PATH})
endforeach()

target_link_libraries(deckgl-bundle INTERFACE probegl mathgl loadersgl lumagl deckgl)

if (DECK_BUILD_SHARED_LIBS)
    set_target_properties(deckgl-bundle PROPERTIES SOVERSION 1)
endif()

############
# EXAMPLES #
############

if (DECK_BUILD_EXAMPLES)
    add_subdirectory(cpp/examples)
endif()

#########
# TESTS #
#########

if (DECK_BUILD_TESTS)
    # Depedencies
    include(CTest)
    include(GoogleTest)

    find_package(GTest CONFIG REQUIRED)

    # Add 'deckgl-bundle-tests' executable
    add_executable(deckgl-bundle-tests)

    # Add test source files from all the modules
    # NOTE: Sources specified by a target contain paths relative to that specific target
    foreach(i RANGE ${DECK_MODULE_COUNT})
        list(GET DECK_MODULES ${i} DECK_MODULE)
        list(GET DECK_MODULE_PATHS ${i} DECK_MODULE_PATH)

        get_target_property(DECK_MODULE_TESTS ${DECK_MODULE} DECK_TEST_SOURCES)
        list(TRANSFORM DECK_MODULE_TESTS PREPEND ${DECK_MODULE_PATH})
        target_sources(deckgl-bundle-tests PRIVATE ${DECK_MODULE_TESTS})
    endforeach()

    target_sources(deckgl-bundle-tests PRIVATE cpp/tests/main.cpp)

    target_compile_options(deckgl-bundle-tests PRIVATE ${DECK_COMPILE_FLAGS})
    target_link_libraries(deckgl-bundle-tests PRIVATE ${DECK_LINK_FLAGS} GTest::gtest deckgl-bundle)

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
    # Gather source file lists for all the targets
    # NOTE: Sources specified by a target contain paths relative to that specific target
    foreach(i RANGE ${DECK_MODULE_COUNT})
        list(GET DECK_MODULES ${i} DECK_MODULE)
        list(GET DECK_MODULE_PATHS ${i} DECK_MODULE_PATH)

        get_target_property(DECK_MODULE_SOURCES ${DECK_MODULE} DECK_ALL_SOURCES)
        list(TRANSFORM DECK_MODULE_SOURCES PREPEND ${DECK_MODULE_PATH})
        list(APPEND DECK_SOURCE_LIST ${DECK_MODULE_SOURCES})
    endforeach()

    # Format
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_PATH}
        -style=file
        -i
        ${SOURCE_LIST}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Formatting sources"
        )
    # Always do formatting
    add_dependencies(deckgl-bundle format)
elseif (NOT CLANG_FORMAT_PATH)
    message(WARNING "clang-format was not detected, so automatic source code reformatting is disabled")
endif()
