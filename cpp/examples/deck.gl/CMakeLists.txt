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

set(EXAMPLE_SOURCE_FILES
    stdinout.cpp
    flight-paths.cc
    manhattan-population.cc
    texture-render.cc
    vancouver-blocks.cc
    )
set(EXAMPLE_DATA_FILES
    ${DECK_DATA_PATH}/examples/flight-paths/heathrow-flights.ndjson
    ${DECK_DATA_PATH}/examples/flight-paths/airports.ndjson
    ${DECK_DATA_PATH}/examples/manhattan-population/manhattan.ndjson
    ${DECK_DATA_PATH}/examples/vancouver-blocks/vancouver-blocks-simplified.ndjson
    )

# Create an executable for each example from the list
foreach(EXAMPLE_SOURCE_FILE ${EXAMPLE_SOURCE_FILES})
    get_filename_component(TARGET_NAME ${EXAMPLE_SOURCE_FILE} NAME_WE)
    add_executable(${TARGET_NAME} ${EXAMPLE_SOURCE_FILE})
    
    target_link_libraries(${TARGET_NAME} PRIVATE ${DECK_LINK_FLAGS} deck.gl loaders.gl)

    # Installation
    install(TARGETS ${TARGET_NAME}
        RUNTIME
            DESTINATION examples/deck.gl
            COMPONENT Examples
        BUNDLE
            DESTINATION examples/deck.gl
            COMPONENT Examples
        )
endforeach()

# Copy supporting data files
foreach(EXAMPLE_DATA_FILE ${EXAMPLE_DATA_FILES})
    get_filename_component(EXAMPLE_DATA_FILE_NAME ${EXAMPLE_DATA_FILE} NAME)
    configure_file(${EXAMPLE_DATA_FILE} ${CMAKE_CURRENT_BINARY_DIR}/data/${EXAMPLE_DATA_FILE_NAME} COPYONLY)
endforeach()

# Add example files to global DECK_EXAMPLE_FILES property
# NOTE: We transform relative paths to absolute paths before appending
set(FILES_TO_ADD ${EXAMPLE_SOURCE_FILES})
list(TRANSFORM FILES_TO_ADD PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
set_property(GLOBAL APPEND PROPERTY DECK_EXAMPLE_FILES ${FILES_TO_ADD})
