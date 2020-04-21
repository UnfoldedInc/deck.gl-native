// Copyright (c) 2020 Unfolded Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// Port of https://github.com/visgl/deck.gl/tree/master/examples/website/line

#include <arrow/io/memory.h>

#include "deck.gl/core.h"
#include "deck.gl/layers.h"
#include "loaders.gl/json.h"

// TODO(ilija@unfolded.ai): These should be loaded in a more efficient way if we figure out the best way to bundle them
#include "./data/heathrow-flights.h"
//#include "./data/airports.h"

using namespace deckgl;

namespace {

loadersgl::JSONLoader jsonLoader;

auto createLineLayer() -> std::shared_ptr<LineLayer::Props> {
  auto lineLayerProps = std::make_shared<LineLayer::Props>();
  lineLayerProps->id = "flight-paths";
  lineLayerProps->opacity = 0.8;
  lineLayerProps->getSourcePosition = [](const Row &row) -> mathgl::Vector3<float> {
    return row.getFloatVector3("start");
  };
  lineLayerProps->getTargetPosition = [](const Row &row) -> mathgl::Vector3<float> {
    return row.getFloatVector3("end");
  };
  lineLayerProps->getColor = [](const Row &row) -> mathgl::Vector4<float> {
    float z = row.getFloatVector3("start").y;
    float r = z / 10000;
    return mathgl::Vector4{255 * (1 - r * 2), 128 * r, 255 * r, 255 * (1 - r)};
  };
  lineLayerProps->getWidth = [](const Row &row) -> float { return 3.0; };
  lineLayerProps->data = jsonLoader.loadTable(std::make_shared<arrow::io::BufferReader>(heathrowFlights));

  return lineLayerProps;
}

auto createInitialViewState() -> std::shared_ptr<ViewState> {
  auto initialViewState = std::make_shared<ViewState>();
  initialViewState->latitude = 47.65;
  initialViewState->longitude = 7.0;
  initialViewState->zoom = 4.5;
  initialViewState->pitch = 50.0;
  initialViewState->bearing = 0.0;

  return initialViewState;
}

}  // anonymous namespace

int main() {
  auto deckProps = std::make_shared<Deck::Props>();
  deckProps->layers = {createLineLayer()};
  deckProps->initialViewState = createInitialViewState();
  deckProps->views = {std::make_shared<MapView>()};

  auto deck = std::make_shared<Deck>(deckProps);
  deck->run();

  return 0;
}
