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

add_library(math.gl)
set_target_properties(math.gl PROPERTIES VERSION ${DECK_VERSION})
set_target_properties(math.gl PROPERTIES SOVERSION ${DECK_SO_VERSION})

# File lists that are maintained manually
set(HEADER_FILE_LIST
    core.h
    core/src/core.h
    core/src/rectangle.h
    web-mercator.h
    web-mercator/src/web-mercator-utils.h
    )
set(SOURCE_FILE_LIST
    web-mercator/src/web-mercator-utils.cc
    )
set(TESTS_HEADER_FILE_LIST
    web-mercator/test/sample-viewports.h
    )
set(TESTS_SOURCE_FILE_LIST
    core/test/core-test.cc
    web-mercator/test/web-mercator-utils-test.cc
    )

target_link_libraries(math.gl PUBLIC ${DECK_CONFIG_LIBRARY})

# Specify sources that'll be compiled
target_sources(math.gl PRIVATE ${SOURCE_FILE_LIST})

# Export source list of this module
# NOTE: We transform relative paths to absolute paths before exporting
set(MODULE_SOURCE_FILES
    ${HEADER_FILE_LIST} ${SOURCE_FILE_LIST}
    ${TESTS_HEADER_FILE_LIST} ${TESTS_SOURCE_FILE_LIST}
    )
list(TRANSFORM MODULE_SOURCE_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
set_property(TARGET math.gl PROPERTY DECK_ALL_SOURCES ${MODULE_SOURCE_FILES})

set(MODULE_TEST_FILES ${TESTS_SOURCE_FILE_LIST})
list(TRANSFORM MODULE_TEST_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
set_property(TARGET math.gl PROPERTY DECK_TEST_SOURCES ${MODULE_TEST_FILES})

# Installation
install(TARGETS math.gl
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
            DESTINATION include/math.gl/${HEADER_DIRECTORY}
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
    target_sources(math.gl PRIVATE ${HEADER_FILE_LIST})

    set_target_properties(math.gl PROPERTIES
        FRAMEWORK ${DECK_USE_FRAMEWORKS}
        FRAMEWORK_VERSION ${DECK_VERSION}
        MACOSX_FRAMEWORK_IDENTIFIER ai.unfolded.mathgl
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${DECK_VERSION}
        )
endif()
