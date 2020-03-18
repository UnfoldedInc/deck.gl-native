# Inspired by https://github.com/uber/h3/blob/master/CMakeLists.txt
# and by https://github.com/google/s2geometry/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

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

## math.gl
set(MATHGL_CORE_HEADER_FILES
    cpp/modules/math.gl/core.h
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
set(LUMAGL_CORE_HEADER_FILES
    cpp/modules/luma.gl/core.h
    )
set(LUMAGL_CORE_SOURCE_FILES
    )
set(LUMAGL_CORE_TEST_FILES
    )

## deck.gl/json
set(DECKGL_JSON_HEADER_FILES
    cpp/modules/deck.gl/json.h
    cpp/modules/deck.gl/json/src/converter/json-types.h
    cpp/modules/deck.gl/json/src/converter/json-types-mathgl.h
    cpp/modules/deck.gl/json/src/converter/json-converter.h
    cpp/modules/deck.gl/json/src/component/component.h
)

set(DECKGL_JSON_SOURCE_FILES
    cpp/modules/deck.gl/json/src/converter/json-types.cpp
    cpp/modules/deck.gl/json/src/converter/json-converter.cpp
    cpp/modules/deck.gl/json/src/component/component.cpp
)
set(DECKGL_JSON_TEST_FILES
    cpp/modules/deck.gl/json/test/json-converter-test.cpp
    cpp/modules/deck.gl/json/test/json-types-test.cpp
)

## deck.gl/core
set(DECKGL_CORE_HEADER_FILES
    cpp/modules/deck.gl/core.h
    cpp/modules/deck.gl/core/src/lib/constants.h
    cpp/modules/deck.gl/core/src/lib/deck.h
    cpp/modules/deck.gl/core/src/lib/layer.h
    cpp/modules/deck.gl/core/src/lib/layer-manager.h
    cpp/modules/deck.gl/core/src/lib/layer-state.h
    cpp/modules/deck.gl/core/src/lib/view-manager.h
    cpp/modules/deck.gl/core/src/viewports/viewport.h
    )
set(DECKGL_CORE_SOURCE_FILES
    cpp/modules/deck.gl/core/src/lib/deck.cpp
    cpp/modules/deck.gl/core/src/lib/layer.cpp
    cpp/modules/deck.gl/core/src/lib/layer-manager.cpp
    cpp/modules/deck.gl/core/src/viewports/viewport.cpp
    )
set(DECKGL_CORE_TEST_FILES
    cpp/modules/deck.gl/core/test/lib/layer-test.cpp
    cpp/modules/deck.gl/core/test/lib/layer-manager-test.cpp
    cpp/modules/deck.gl/core/test/viewports/viewport-test.cpp
    )

## deck.gl/layers
set(DECKGL_LAYERS_HEADER_FILES
    cpp/modules/deck.gl/layers.h
    cpp/modules/deck.gl/layers/src/line-layer/line-layer.h
    cpp/modules/deck.gl/layers/src/scatterplot-layer/scatterplot-layer.h
    )
set(DECKGL_LAYERS_SOURCE_FILES
    cpp/modules/deck.gl/layers/src/line-layer/line-layer.cpp
    cpp/modules/deck.gl/layers/src/scatterplot-layer/scatterplot-layer.cpp
    )
set(DECKGL_LAYERS_TEST_FILES
    cpp/modules/deck.gl/layers/test/line-layer-test.cpp
    cpp/modules/deck.gl/layers/test/scatterplot-layer-test.cpp
    )

# Define source file lists per type
set(LIB_HEADER_FILES
    ${MATHGL_CORE_HEADER_FILES}
    ${LOADERSGL_CSV_HEADER_FILES} ${LOADERSGL_JSON_HEADER_FILES}
    ${LUMAGL_CORE_HEADER_FILES}
    ${DECKGL_JSON_HEADER_FILES} ${DECKGL_CORE_HEADER_FILES} ${DECKGL_LAYERS_HEADER_FILES}
    )
set(LIB_SOURCE_FILES
    ${MATHGL_CORE_SOURCE_FILES}
    ${LOADERSGL_CSV_SOURCE_FILES} ${LOADERSGL_JSON_SOURCE_FILES}
    ${LUMAGL_CORE_SOURCE_FILES}
    ${DECKGL_JSON_SOURCE_FILES} ${DECKGL_CORE_SOURCE_FILES} ${DECKGL_LAYERS_SOURCE_FILES}
    )
set(TEST_SOURCE_FILES
    ${MATHGL_CORE_TEST_FILES}
    ${LOADERSGL_CSV_TEST_FILES} ${LOADERSGL_JSON_TEST_FILES}
    ${LUMAGL_CORE_TEST_FILES}
    ${DECKGL_JSON_TEST_FILES} ${DECKGL_CORE_TEST_FILES} ${DECKGL_LAYERS_TEST_FILES}
    cpp/tests/main.cpp
    )
SET(TEST_HEADER_FILES
    ${MATHGL_CORE_TEST_HEADER_FILES}
    )

set(ALL_SOURCE_FILES
    ${LIB_SOURCE_FILES} ${TEST_SOURCE_FILES} ${LIB_HEADER_FILES} ${TEST_HEADER_FILES})

# Dependencies
find_package(jsoncpp CONFIG REQUIRED)

# TODO: Remove. Workaround for not being able to install apache through vcpkg and using Homebrew instead
include_directories(/usr/local/Cellar/apache-arrow/0.16.0/include)
link_directories(/usr/local/Cellar/apache-arrow/0.16.0/lib)
find_package(Arrow CONFIG REQUIRED)

# Build the actual library file
add_library(deck.gl ${LIB_SOURCE_FILES})

target_compile_options(deck.gl PRIVATE ${DECK_COMPILE_FLAGS})
target_link_libraries(deck.gl PRIVATE ${DECK_LINK_FLAGS} jsoncpp_lib arrow)

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
