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

add_library(loaders.gl)

# File lists that are maintained manually
set(HEADER_FILE_LIST
    csv/src/csv-loader.h
    json/src/json-loader.h
    )
set(SOURCE_FILE_LIST
    csv/src/csv-loader.cpp
    json/src/json-loader.cpp
    )
set(TESTS_SOURCE_FILE_LIST
    csv/test/csv-loader-test.cpp
    json/test/json-loader-test.cpp
    )

# Some of the include paths are relative to modules directory
target_include_directories(loaders.gl PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/cpp/modules/>)

# Dependencies
find_package(arrow CONFIG REQUIRED)    # Binary columnar tables
target_link_libraries(loaders.gl PUBLIC arrow)

# Specify sources that'll be compiled
target_sources(loaders.gl PRIVATE ${SOURCE_FILE_LIST})

# Add source files to parent-defined lists
list(APPEND DECK_ALL_SOURCES
    ${HEADER_FILE_LIST} ${SOURCE_FILE_LIST}
    ${TESTS_SOURCE_FILE_LIST}
    )
list(APPEND DECK_TEST_SOURCES ${TESTS_SOURCE_FILE_LIST})
