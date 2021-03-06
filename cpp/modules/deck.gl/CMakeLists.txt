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

add_library(deck.gl)
set_target_properties(deck.gl PROPERTIES VERSION ${DECK_VERSION})
set_target_properties(deck.gl PROPERTIES SOVERSION ${DECK_SO_VERSION})

# File lists that are maintained manually

## deck.gl/json
set(JSON_HEADER_FILES
    json.h
    json/src/json.h
    json/src/converter/json-types.h
    json/src/converter/json-types-mathgl.h
    json/src/converter/json-converter.h
    json/src/json-object/json-object.h
    )
set(JSON_SOURCE_FILES
    json/src/converter/json-types.cc
    json/src/converter/json-converter.cc
    json/src/json-object/json-object.cc
    )
set(JSON_TEST_SOURCE_FILES
    json/test/json-object-test.cc
    json/test/json-converter-test.cc
    json/test/json-types-test.cc
    )

## deck.gl/core
set(CORE_HEADER_FILES
    core.h
    core/src/core.h
    core/src/lib/attribute/attribute-manager.h
    core/src/lib/component.h
    core/src/lib/constants.h
    core/src/lib/deck.h
    core/src/lib/earcut.hpp
    core/src/lib/layer.h
    core/src/lib/layer-context.h
    core/src/lib/layer-manager.h
    core/src/lib/layer-state.h
    core/src/lib/view-manager.h
    core/src/viewports/viewport.h
    core/src/viewports/web-mercator-viewport.h
    core/src/views/map-view.h
    core/src/views/view.h
    core/src/views/view-state.h
    core/src/arrow/row.h
    core/src/arrow/arrow-mapper.h
    core/src/shaderlib/project/viewport-uniforms.h
    )
set(CORE_SOURCE_FILES
    core/src/core.cc
    core/src/lib/attribute/attribute-manager.cc
    core/src/lib/component.cc
    core/src/lib/constants.cc
    core/src/lib/deck.cc
    core/src/lib/layer.cc
    core/src/lib/layer-manager.cc
    core/src/lib/view-manager.cc
    core/src/shaderlib/project/viewport-uniforms.cc
    core/src/viewports/viewport.cc
    core/src/viewports/web-mercator-viewport.cc
    core/src/views/map-view.cc
    core/src/views/view.cc
    core/src/views/view-state.cc
    core/src/arrow/row.cc
    core/src/arrow/arrow-mapper.cc
    )
set(CORE_TEST_HEADER_FILES
    core/test/views/map-view-json-data.h
    core/test/views/view-json-data.h
    )
set(CORE_TEST_SOURCE_FILES
    core/test/lib/attribute/attribute-manager-test.cc
    core/test/lib/layer-test.cc
    core/test/lib/layer-manager-test.cc
    core/test/lib/view-manager-test.cc
    core/test/lib/earcut-test.cc
    core/test/shaderlib/project/viewport-uniforms-test.cc
    core/test/viewports/viewport-test.cc
    core/test/viewports/web-mercator-viewport-test.cc
    core/test/views/map-view-test.cc
    core/test/views/view-state-test.cc
    core/test/views/view-test.cc
    core/test/arrow/row-test.cc
    )

## deck.gl/layers
set(LAYERS_HEADER_FILES
    layers.h
    layers/src/layers.h
    layers/src/line-layer/line-layer.h
    layers/src/scatterplot-layer/scatterplot-layer.h
    layers/src/solid-polygon-layer/solid-polygon-layer.h
    )
set(LAYERS_SOURCE_FILES
    layers/src/layers.cc
    layers/src/line-layer/line-layer.cc
    layers/src/scatterplot-layer/scatterplot-layer.cc
    layers/src/solid-polygon-layer/solid-polygon-layer.cc
    )
set(LAYERS_TEST_SOURCE_FILES
    layers/test/line-layer-test.cc
    layers/test/scatterplot-layer-test.cc
    layers/test/solid-polygon-layer-test.cc
    )

set(HEADER_FILE_LIST
    ${JSON_HEADER_FILES}
    ${CORE_HEADER_FILES}
    ${LAYERS_HEADER_FILES}
    )
set(SOURCE_FILE_LIST
    ${JSON_SOURCE_FILES}
    ${CORE_SOURCE_FILES}
    ${LAYERS_SOURCE_FILES}
    )
set(TESTS_HEADER_FILE_LIST
    ${CORE_TEST_HEADER_FILES}
    )
set(TESTS_SOURCE_FILE_LIST
    ${JSON_TEST_SOURCE_FILES}
    ${CORE_TEST_SOURCE_FILES}
    ${LAYERS_TEST_SOURCE_FILES}
    )

# We're using pre-built dependencies from our dependency submodule
# NO_DEFAULT_PATH and NO_CMAKE_FIND_ROOT_PATH make it so other lookup paths are ignored
find_library(arrow_LIB arrow PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
find_library(jsoncpp_LIB jsoncpp PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)

target_link_libraries(deck.gl PUBLIC ${DECK_LINK_FLAGS} ${DECK_SHARED_LIB}
    probe.gl math.gl luma.gl ${arrow_LIB} ${jsoncpp_LIB})

target_compile_options(deck.gl PUBLIC ${DECKGL_COMPILE_FLAGS})
# Specify sources that'll be compiled
target_sources(deck.gl PRIVATE ${SOURCE_FILE_LIST})

# Export source list of this module
# NOTE: We transform relative paths to absolute paths before exporting
set(MODULE_SOURCE_FILES
    ${HEADER_FILE_LIST} ${SOURCE_FILE_LIST}
    ${TESTS_HEADER_FILE_LIST} ${TESTS_SOURCE_FILE_LIST}
    )
list(TRANSFORM MODULE_SOURCE_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
set_property(TARGET deck.gl PROPERTY DECK_ALL_SOURCES ${MODULE_SOURCE_FILES})

set(MODULE_TEST_FILES ${TESTS_SOURCE_FILE_LIST})
list(TRANSFORM MODULE_TEST_FILES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
set_property(TARGET deck.gl PROPERTY DECK_TEST_SOURCES ${MODULE_TEST_FILES})

# Installation
install(TARGETS deck.gl
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
            DESTINATION include/deck.gl/${HEADER_DIRECTORY}
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
    target_sources(deck.gl PRIVATE ${HEADER_FILE_LIST})

    set_target_properties(deck.gl PROPERTIES
        FRAMEWORK ${DECK_USE_FRAMEWORKS}
        FRAMEWORK_VERSION ${DECK_VERSION}
        MACOSX_FRAMEWORK_IDENTIFIER ai.unfolded.deckgl
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${DECK_VERSION}
        )
endif()
