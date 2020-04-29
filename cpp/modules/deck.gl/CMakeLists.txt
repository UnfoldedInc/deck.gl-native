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
    json/src/converter/json-types.cpp
    json/src/converter/json-converter.cpp
    json/src/json-object/json-object.cpp
    )
set(JSON_TEST_SOURCE_FILES
    json/test/json-object-test.cpp
    json/test/json-converter-test.cpp
    json/test/json-types-test.cpp
    )

## deck.gl/core
set(CORE_HEADER_FILES
    core.h
    core/src/core.h
    core/src/lib/attribute/attribute-manager.h
    core/src/lib/component.h
    core/src/lib/constants.h
    core/src/lib/deck.h
    core/src/lib/layer.h
    core/src/lib/layer-manager.h
    core/src/lib/layer-state.h
    core/src/lib/view-manager.h
    core/src/shaderlib/project/viewport-uniforms.h
    core/src/viewports/viewport.h
    core/src/viewports/web-mercator-viewport.h
    core/src/views/view.h
    core/src/views/view-state.h
    core/src/arrow/row.h
    core/src/arrow/arrow-mapper.h
    )
set(CORE_SOURCE_FILES
    core/src/core.cpp
    core/src/lib/attribute/attribute-manager.cc
    core/src/lib/constants.cpp
    core/src/lib/deck.cpp
    core/src/lib/layer.cpp
    core/src/lib/layer-manager.cpp
    core/src/lib/view-manager.cpp
    core/src/shaderlib/project/viewport-uniforms.cpp
    core/src/viewports/viewport.cpp
    core/src/viewports/web-mercator-viewport.cpp
    core/src/views/map-view.cpp
    core/src/views/view.cpp
    core/src/views/view-state.cpp
    core/src/arrow/row.cc
    core/src/arrow/arrow-mapper.cc
    )
set(CORE_TEST_HEADER_FILES
    core/test/views/map-view-json-data.h
    core/test/views/view-json-data.h
    )
set(CORE_TEST_SOURCE_FILES
    core/test/lib/attribute/attribute-manager-test.cc
    core/test/lib/layer-test.cpp
    core/test/lib/layer-manager-test.cpp
    core/test/lib/view-manager-test.cpp
    core/test/shaderlib/project/viewport-uniforms-test.cpp
    core/test/viewports/viewport-test.cpp
    core/test/viewports/web-mercator-viewport-test.cpp
    core/test/views/map-view-test.cpp
    core/test/views/view-state-test.cpp
    core/test/views/view-test.cpp
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
    layers/src/layers.cpp
    layers/src/line-layer/line-layer.cpp
    layers/src/scatterplot-layer/scatterplot-layer.cpp
    layers/src/solid-polygon-layer/solid-polygon-layer.cpp
    )
set(LAYERS_TEST_SOURCE_FILES
    layers/test/line-layer-test.cpp
    layers/test/scatterplot-layer-test.cpp
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

find_library(arrow_LIB arrow PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH)
find_library(jsoncpp_LIB jsoncpp PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH)
find_library(fmt_LIB fmt PATH ${DECK_DEPS_PATH} NO_DEFAULT_PATH)

target_link_libraries(deck.gl PUBLIC ${DECK_LINK_FLAGS} ${DECK_SHARED_LIB}
    probe.gl math.gl luma.gl ${arrow_LIB} ${jsoncpp_LIB} ${fmt_LIB})

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
