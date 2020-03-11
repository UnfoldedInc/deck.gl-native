# Inspired by https://github.com/uber/h3/blob/master/CMakeLists.txt
# and by https://github.com/google/s2geometry/blob/master/CMakeLists.txt

cmake_minimum_required(VERSION 3.8)

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

# Define source file lists
set(LIB_SOURCE_FILES
    cpp/src/main.cpp)
set(LIB_HEADER_FILES
    cpp/src/main.h)
set(TEST_SOURCE_FILES
    cpp/tests/main.cpp)
SET(TEST_HEADER_FILES
    )

set(ALL_SOURCE_FILES
    ${LIB_SOURCE_FILES} ${TEST_SOURCE_FILES} ${LIB_HEADER_FILES} ${TEST_HEADER_FILES})

# Dependencies
find_package(jsoncpp CONFIG REQUIRED)

# Build the actual library file
add_library(deck.gl ${LIB_SOURCE_FILES})

target_compile_options(deck.gl PRIVATE ${DECK_COMPILE_FLAGS})
target_link_libraries(deck.gl PRIVATE ${DECK_LINK_FLAGS} jsoncpp_lib)

if(BUILD_SHARED_LIBS)
    set_target_properties(deck.gl PROPERTIES SOVERSION 1)
endif()

target_include_directories(deck.gl PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cpp/src/>)

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

# Add each source file as a test executable
foreach(test_src ${TEST_SOURCE_FILES})
    get_filename_component(test ${test_src} NAME_WE)
    add_executable(${test} ${test_src})
    target_link_libraries(${test} PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main deck.gl)
    add_test(NAME ${test} COMMAND ${TEST_WRAPPER} "$<TARGET_FILE:${test}>")
endforeach()
