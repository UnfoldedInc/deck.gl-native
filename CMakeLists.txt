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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(deck.gl LANGUAGES C CXX)

set(DECK_COMPILE_FLAGS "")
set(DECK_LINK_FLAGS "")
if(NOT WIN32)
    list(APPEND DECK_COMPILE_FLAGS -Wall)

    list(APPEND DECK_COMPILE_FLAGS $<$<CONFIG:Debug>:-gdwarf-2 -g3 -O0 -fno-inline -fno-eliminate-unused-debug-types>)

    if(ENABLE_COVERAGE)
        list(APPEND DECK_COMPILE_FLAGS $<$<CONFIG:Debug>:--coverage>)
        # --coverage is not passed to the linker, so this option is needed
        # to fully enable coverage.
        list(APPEND DECK_LINK_FLAGS $<$<CONFIG:Debug>:--coverage>)
    endif()

    # Not a good idea to leave this on because it will generate errors on different compilers
    # that aren't tested in our CI. Can be useful for developers in order to keep code clean
    # of warnings.
    option(WARNINGS_AS_ERRORS "Warnings are treated as errors" OFF)
    if(WARNINGS_AS_ERRORS)
        list(APPEND DECK_COMPILE_FLAGS -Werror)
    endif()
endif()

include(CMakeDependentOption)
include(CTest)

# Define source file lists per library

## probe.gl
set(PROBEGL_CORE_HEADER_FILES
    cpp/modules/probe.gl/core.h
    cpp/modules/probe.gl/core/src/compiler.h
    cpp/modules/probe.gl/core/src/platform.h
    cpp/modules/probe.gl/core/src/assert.h
    cpp/modules/probe.gl/core/src/log.h
    cpp/modules/probe.gl/core/src/system-utils.h
    cpp/modules/probe.gl/core/src/timer.h
)
set(PROBEGL_CORE_SOURCE_FILES
    cpp/modules/probe.gl/core/src/assert.cpp
    cpp/modules/probe.gl/core/src/log.cpp
    cpp/modules/probe.gl/core/src/system-utils.cpp
    cpp/modules/probe.gl/core/src/timer.cpp
)
set(PROBEGL_CORE_TEST_FILES
    cpp/modules/probe.gl/core/test/timer-test.cpp
)


## math.gl
set(MATHGL_CORE_HEADER_FILES
    cpp/modules/math.gl/core.h
    cpp/modules/math.gl/core/src/core.h
    cpp/modules/math.gl/core/src/rectangle.h
    cpp/modules/math.gl/web-mercator/src/web-mercator-utils.h
    )
set(MATHGL_CORE_SOURCE_FILES
    cpp/modules/math.gl/web-mercator/src/web-mercator-utils.cpp
    )
set(MATHGL_CORE_TEST_HEADER_FILES
    cpp/modules/math.gl/web-mercator/test/sample-viewports.h
    )
set(MATHGL_CORE_TEST_FILES
    cpp/modules/math.gl/core/test/core-test.cpp
    cpp/modules/math.gl/web-mercator/test/web-mercator-utils-test.cpp
    )

## loaders.gl/csv
set(LOADERSGL_CSV_HEADER_FILES
    cpp/modules/loaders.gl/csv/src/csv-loader.h
)
set(LOADERSGL_CSV_SOURCE_FILES
    cpp/modules/loaders.gl/csv/src/csv-loader.cpp
)
set(LOADERSGL_CSV_TEST_FILES
    cpp/modules/loaders.gl/csv/test/csv-loader-test.cpp
)

## loaders.gl/json
set(LOADERSGL_JSON_HEADER_FILES
    cpp/modules/loaders.gl/json/src/json-loader.h
)
set(LOADERSGL_JSON_SOURCE_FILES
    cpp/modules/loaders.gl/json/src/json-loader.cpp
)
set(LOADERSGL_JSON_TEST_FILES
    cpp/modules/loaders.gl/json/test/json-loader-test.cpp
)

## luma.gl
set(LUMAGL_WEBGPU_HEADER_FILES
    cpp/modules/luma.gl/webgpu.h
    cpp/modules/luma.gl/webgpu/src/combo-render-pipeline-descriptor.h
    cpp/modules/luma.gl/webgpu/src/shaderc-utils.h
    cpp/modules/luma.gl/webgpu/src/swap-chain-utils.h
    cpp/modules/luma.gl/webgpu/src/webgpu-constants.h
    cpp/modules/luma.gl/webgpu/src/webgpu-utils.h
    cpp/modules/luma.gl/webgpu/src/webgpu-helpers.h
    cpp/modules/luma.gl/webgpu/src/terrible-command-buffer.h
    cpp/modules/luma.gl/webgpu/src/backends/backend-binding.h
    )
set(LUMAGL_WEBGPU_SOURCE_FILES
    cpp/modules/luma.gl/webgpu/src/combo-render-pipeline-descriptor.cpp
    cpp/modules/luma.gl/webgpu/src/shaderc-utils.cpp
    cpp/modules/luma.gl/webgpu/src/terrible-command-buffer.cpp
    cpp/modules/luma.gl/webgpu/src/webgpu-utils.cpp
    cpp/modules/luma.gl/webgpu/src/webgpu-helpers.cpp
    cpp/modules/luma.gl/webgpu/src/backends/backend-binding.cpp
    # TODO: These should be included on per-platform basis. See src/utils/CMakeLists.txt in dawn repo
    cpp/modules/luma.gl/webgpu/src/backends/metal-binding.mm
    #cpp/modules/luma.gl/webgpu/src/backends/vulkan-binding.cpp
    #cpp/modules/luma.gl/webgpu/src/backends/d3d12-binding.cpp
    #cpp/modules/luma.gl/webgpu/src/backends/null-binding.cpp
    #cpp/modules/luma.gl/webgpu/src/backends/opengl-binding.cpp
    )
set(LUMAGL_WEBGPU_TEST_FILES
    cpp/modules/luma.gl/webgpu/test/webgpu-test.cpp
    )

set(LUMAGL_CORE_HEADER_FILES
    cpp/modules/luma.gl/core.h
    cpp/modules/luma.gl/core/src/animation-loop.h
    cpp/modules/luma.gl/core/src/glfw-animation-loop.h
    cpp/modules/luma.gl/core/src/model.h
    )
set(LUMAGL_CORE_SOURCE_FILES
    cpp/modules/luma.gl/core/src/animation-loop.cpp
    # cpp/modules/luma.gl/core/src/glfw-animation-loop.cpp
    cpp/modules/luma.gl/core/src/model.cpp
    )
set(LUMAGL_CORE_TEST_FILES
    )

## deck.gl/json
set(DECKGL_JSON_HEADER_FILES
    cpp/modules/deck.gl/json.h
    cpp/modules/deck.gl/json/src/json.h
    cpp/modules/deck.gl/json/src/converter/json-types.h
    cpp/modules/deck.gl/json/src/converter/json-types-mathgl.h
    cpp/modules/deck.gl/json/src/converter/json-converter.h
    cpp/modules/deck.gl/json/src/json-object/json-object.h
)

set(DECKGL_JSON_SOURCE_FILES
    cpp/modules/deck.gl/json/src/converter/json-types.cpp
    cpp/modules/deck.gl/json/src/converter/json-converter.cpp
    cpp/modules/deck.gl/json/src/json-object/json-object.cpp
)
set(DECKGL_JSON_TEST_FILES
    cpp/modules/deck.gl/json/test/json-object-test.cpp
    cpp/modules/deck.gl/json/test/json-converter-test.cpp
    cpp/modules/deck.gl/json/test/json-types-test.cpp
)

## deck.gl/core
set(DECKGL_CORE_HEADER_FILES
    cpp/modules/deck.gl/core.h
    cpp/modules/deck.gl/core/src/core.h
    cpp/modules/deck.gl/core/src/lib/attribute/attribute.h
    cpp/modules/deck.gl/core/src/lib/attribute/attribute-manager.h
    cpp/modules/deck.gl/core/src/lib/component.h
    cpp/modules/deck.gl/core/src/lib/constants.h
    cpp/modules/deck.gl/core/src/lib/deck.h
    cpp/modules/deck.gl/core/src/lib/layer.h
    cpp/modules/deck.gl/core/src/lib/layer-manager.h
    cpp/modules/deck.gl/core/src/lib/layer-state.h
    cpp/modules/deck.gl/core/src/lib/view-manager.h
    cpp/modules/deck.gl/core/src/shaderlib/project/viewport-uniforms.h
    cpp/modules/deck.gl/core/src/viewports/viewport.h
    cpp/modules/deck.gl/core/src/viewports/web-mercator-viewport.h
    cpp/modules/deck.gl/core/src/views/view.h
    cpp/modules/deck.gl/core/src/views/view-state.h
    cpp/modules/deck.gl/core/src/arrow/row.h
    cpp/modules/deck.gl/core/src/arrow/arrow-mapper.h
    )
set(DECKGL_CORE_SOURCE_FILES
    cpp/modules/deck.gl/core/src/core.cpp
    cpp/modules/deck.gl/core/src/lib/attribute/attribute-manager.cc
    cpp/modules/deck.gl/core/src/lib/constants.cpp
    cpp/modules/deck.gl/core/src/lib/deck.cpp
    cpp/modules/deck.gl/core/src/lib/layer.cpp
    cpp/modules/deck.gl/core/src/lib/layer-manager.cpp
    cpp/modules/deck.gl/core/src/lib/view-manager.cpp
    cpp/modules/deck.gl/core/src/shaderlib/project/viewport-uniforms.cpp
    cpp/modules/deck.gl/core/src/viewports/viewport.cpp
    cpp/modules/deck.gl/core/src/viewports/web-mercator-viewport.cpp
    cpp/modules/deck.gl/core/src/views/map-view.cpp
    cpp/modules/deck.gl/core/src/views/view.cpp
    cpp/modules/deck.gl/core/src/views/view-state.cpp
    cpp/modules/deck.gl/core/src/arrow/row.cc
    cpp/modules/deck.gl/core/src/arrow/arrow-mapper.cc
    )
set(DECKGL_CORE_TEST_FILES
    cpp/modules/deck.gl/core/test/lib/attribute/attribute-manager-test.cc
    cpp/modules/deck.gl/core/test/lib/layer-test.cpp
    cpp/modules/deck.gl/core/test/lib/layer-manager-test.cpp
    cpp/modules/deck.gl/core/test/lib/view-manager-test.cpp
    cpp/modules/deck.gl/core/test/shaderlib/project/viewport-uniforms-test.cpp
    cpp/modules/deck.gl/core/test/viewports/viewport-test.cpp
    cpp/modules/deck.gl/core/test/viewports/web-mercator-viewport-test.cpp
    cpp/modules/deck.gl/core/test/views/map-view-test.cpp
    cpp/modules/deck.gl/core/test/views/view-state-test.cpp
    cpp/modules/deck.gl/core/test/views/view-test.cpp
    cpp/modules/deck.gl/core/test/arrow/row-test.cc
    )
set(DECKGL_CORE_TEST_HEADER_FILES
    cpp/modules/deck.gl/core/test/views/map-view-json-data.h
    cpp/modules/deck.gl/core/test/views/view-json-data.h
    )

## deck.gl/layers
set(DECKGL_LAYERS_HEADER_FILES
    cpp/modules/deck.gl/layers.h
    cpp/modules/deck.gl/layers/src/layers.h
    cpp/modules/deck.gl/layers/src/line-layer/line-layer.h
    cpp/modules/deck.gl/layers/src/scatterplot-layer/scatterplot-layer.h
    )
set(DECKGL_LAYERS_SOURCE_FILES
    cpp/modules/deck.gl/layers/src/layers.cpp
    cpp/modules/deck.gl/layers/src/line-layer/line-layer.cpp
    cpp/modules/deck.gl/layers/src/scatterplot-layer/scatterplot-layer.cpp
    )
set(DECKGL_LAYERS_TEST_FILES
    cpp/modules/deck.gl/layers/test/line-layer-test.cpp
    cpp/modules/deck.gl/layers/test/scatterplot-layer-test.cpp
    )

## deck.gl examples
set(DECKGL_EXAMPLE_SOURCE_FILES
    cpp/examples/deck.gl/stdinout.cpp
    )
set(DECKGL_EXAMPLE_HEADER_FILES
    )

# Define source file lists per type
set(LIB_HEADER_FILES
    ${PROBEGL_CORE_HEADER_FILES}
    ${MATHGL_CORE_HEADER_FILES}
    ${LOADERSGL_CSV_HEADER_FILES} ${LOADERSGL_JSON_HEADER_FILES}
    ${LUMAGL_CORE_HEADER_FILES} ${LUMAGL_WEBGPU_HEADER_FILES}
    ${DECKGL_JSON_HEADER_FILES} ${DECKGL_CORE_HEADER_FILES} ${DECKGL_LAYERS_HEADER_FILES}
    )
set(LIB_SOURCE_FILES
    ${PROBEGL_CORE_SOURCE_FILES}
    ${MATHGL_CORE_SOURCE_FILES}
    ${LOADERSGL_CSV_SOURCE_FILES} ${LOADERSGL_JSON_SOURCE_FILES}
    ${LUMAGL_CORE_SOURCE_FILES} ${LUMAGL_WEBGPU_SOURCE_FILES}
    ${DECKGL_JSON_SOURCE_FILES} ${DECKGL_CORE_SOURCE_FILES} ${DECKGL_LAYERS_SOURCE_FILES}
    )
set(TEST_SOURCE_FILES
    ${PROBEGL_CORE_TEST_FILES}
    ${MATHGL_CORE_TEST_FILES}
    ${LOADERSGL_CSV_TEST_FILES} ${LOADERSGL_JSON_TEST_FILES}
    ${LUMAGL_CORE_TEST_FILES} ${LUMAGL_WEBGPU_TEST_FILES}
    ${DECKGL_JSON_TEST_FILES} ${DECKGL_CORE_TEST_FILES} ${DECKGL_LAYERS_TEST_FILES}
    cpp/tests/main.cpp
    )
SET(TEST_HEADER_FILES
    ${DECKGL_CORE_TEST_HEADER_FILES}
    ${MATHGL_CORE_TEST_HEADER_FILES}
    )
SET(EXAMPLE_SOURCE_FILES
    ${DECKGL_EXAMPLE_SOURCE_FILES}
    )
SET(EXAMPLE_HEADER_FILES
    ${DECKGL_EXAMPLE_HEADER_FILES}
    )

set(ALL_SOURCE_FILES
    ${LIB_SOURCE_FILES} ${TEST_SOURCE_FILES} ${EXAMPLE_SOURCE_FILES}
    ${LIB_HEADER_FILES} ${TEST_HEADER_FILES} ${EXAMPLE_HEADER_FILES}
    )

## Dependencies

# TODO: Remove. Workaround for not being able to install apache through vcpkg and using Homebrew instead
# TODO: Workaround for using Homebrew
#  due to not being able to (efficiently or otherwise) install certain modules through vcpkg
include_directories(/usr/local/Cellar/apache-arrow/0.16.0/include)
link_directories(/usr/local/Cellar/apache-arrow/0.16.0/lib)
include_directories(/usr/local/Cellar/gdal/2.4.4_2/include)
link_directories(/usr/local/Cellar/gdal/2.4.4_2/lib)

# General C++ Dependencies
find_package(jsoncpp CONFIG REQUIRED)  # C++ JSON parser
find_package(range-v3 CONFIG REQUIRED) # C++20 ranges on C++ 17.
find_package(fmt CONFIG REQUIRED)      # C++20 fmt on C++17. Link w/ fmt::fmt or fmt::fmt-header-only
find_package(Arrow CONFIG REQUIRED)    # Binary columnar tables

# luma.gl Graphics Dependencies
# dawn: Note: installed separately, currently headers checked in
# find_package(shaderc CONFIG REQUIRED)  # GLSL to SPIR-V compiler (for dynamic shaders)
find_package(glfw3 CONFIG REQUIRED)    # Windowing integration for dawn

## Library Targets

# Build the actual library file
add_library(deck.gl ${LIB_SOURCE_FILES})

# TODO: dawn can be included but is not linked yet.
target_include_directories(deck.gl PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cpp/deps/dawn/include/>)
target_compile_options(deck.gl PUBLIC ${DECK_COMPILE_FLAGS})
target_link_libraries(deck.gl PUBLIC ${DECK_LINK_FLAGS} jsoncpp_lib range-v3::range-v3 fmt::fmt arrow glfw)

if(BUILD_SHARED_LIBS)
    set_target_properties(deck.gl PROPERTIES SOVERSION 1)
endif()

target_include_directories(deck.gl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cpp/modules/>)

# Automatic code formatting
find_program(CLANG_FORMAT_PATH clang-format)
cmake_dependent_option(
    ENABLE_FORMAT "Enable running clang-format before compiling" ON
    "CLANG_FORMAT_PATH" OFF)
if(ENABLE_FORMAT)
    # Format
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_PATH}
        -style=file
        -i
        ${ALL_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Formatting sources"
        )
    # Always do formatting
    add_dependencies(deck.gl format)
elseif(NOT CLANG_FORMAT_PATH)
    message(WARNING "clang-format was not detected, "
                    "so automatic source code reformatting is disabled")
endif()

# examples
foreach(example_cpp_file ${DECKGL_EXAMPLE_SOURCE_FILES})
    get_filename_component(example ${example_cpp_file} NAME_WE)
    add_executable(${example} ${example_cpp_file})
    target_link_libraries(${example} PRIVATE deck.gl)
endforeach()

# Testing
enable_testing()
include(TestWrapValgrind)
find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

option(ENABLE_COVERAGE "Enable coverage instrumentation" ON)
if(ENABLE_COVERAGE)
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

# Add 'tests' executable
add_executable(tests ${TEST_SOURCE_FILES})

target_compile_options(tests PRIVATE ${DECK_COMPILE_FLAGS})
target_link_libraries(tests PRIVATE ${DECK_LINK_FLAGS} GTest::gtest deck.gl)

# Extract registered Google Tests from the executable and add them to ctest
# https://cmake.org/cmake/help/v3.10/module/GoogleTest.html
gtest_discover_tests(tests)
