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

add_library(probe.gl)

# File lists that are maintained manually
set(HEADER_FILE_LIST
    core.h
    core/src/compiler.h
    core/src/platform.h
    core/src/assert.h
    core/src/log.h
    core/src/system-utils.h
    core/src/timer.h
    )
set(SOURCE_FILE_LIST
    core/src/assert.cpp
    core/src/log.cpp
    core/src/system-utils.cpp
    core/src/timer.cpp
    )
set(TESTS_SOURCE_FILE_LIST
    core/test/timer-test.cpp
    )

target_link_libraries(probe.gl PUBLIC ${DECK_CONFIG_LIBRARY})

# Specify sources that'll be compiled
target_sources(probe.gl PRIVATE ${SOURCE_FILE_LIST})

# Export source list of this module
# NOTE: We transform relative paths to absolute paths before exporting
set(MODULE_SOURCE_FILES
    ${HEADER_FILE_LIST} ${SOURCE_FILE_LIST}
    ${TESTS_SOURCE_FILE_LIST}
    )
list(TRANSFORM MODULE_SOURCE_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
set_property(TARGET probe.gl PROPERTY DECK_ALL_SOURCES ${MODULE_SOURCE_FILES})

set(MODULE_TEST_FILES ${TESTS_SOURCE_FILE_LIST})
list(TRANSFORM MODULE_TEST_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
set_property(TARGET probe.gl PROPERTY DECK_TEST_SOURCES ${MODULE_TEST_FILES})
