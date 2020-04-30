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

#include <arrow/filesystem/localfs.h>

#include "deck.gl/core.h"
#include "deck.gl/layers.h"
#include "loaders.gl/json.h"

using namespace deckgl;

loadersgl::JSONLoader jsonLoader;
auto fileSystem = std::make_shared<arrow::fs::LocalFileSystem>();

auto createInitialViewState() -> std::shared_ptr<ViewState> {
  auto initialViewState = std::make_shared<ViewState>();
  initialViewState->latitude = 47.65;
  initialViewState->longitude = 7.0;
  initialViewState->zoom = 4.5;
  initialViewState->pitch = 50.0;
  initialViewState->bearing = 0.0;

  return initialViewState;
}

auto createLineLayer(const std::string &dataPath) -> std::shared_ptr<LineLayer::Props> {
  auto lineLayerProps = std::make_shared<LineLayer::Props>();
  lineLayerProps->id = "flight-paths";
  lineLayerProps->opacity = 0.8f;
  lineLayerProps->getSourcePosition = [](const Row &row) -> mathgl::Vector3<float> {
    // NOTE: Decimal array data is being loaded as double
    auto startPositions = row.getDoubleVector3("start");
    return mathgl::Vector3<float>{startPositions};
  };
  lineLayerProps->getTargetPosition = [](const Row &row) -> mathgl::Vector3<float> {
    // NOTE: Decimal array data is being loaded as double
    auto endPositions = row.getDoubleVector3("end");
    return mathgl::Vector3<float>{endPositions};
  };
  lineLayerProps->getColor = [](const Row &row) -> mathgl::Vector4<float> {
    // NOTE: Decimal array data is being loaded as double
    float z = static_cast<float>(row.getDoubleVector3("start").z);
    float r = z / 10000.0f;
    return {255.0f * (1.0f - r * 2.0f), 128.0f * r, 255.0f * r, 255.0f * (1.0f - r)};
  };
  lineLayerProps->getWidth = [](const Row &row) -> float { return 3.0f; };
  lineLayerProps->data = jsonLoader.loadTable(fileSystem->OpenInputStream(dataPath).ValueOrDie());

  return lineLayerProps;
}

int main(int argc, const char *argv[]) {
  // Get data file paths relative to working directory
  auto programPath = std::string{argv[0]};
  auto programDirectory = programPath.erase(programPath.find_last_of("/"));
  auto flightDataPath = programDirectory.append("/data/heathrow-flights.ndjson");

  auto deckProps = std::make_shared<Deck::Props>();
  deckProps->layers = {createLineLayer(flightDataPath)};
  deckProps->initialViewState = createInitialViewState();
  deckProps->views = {std::make_shared<MapView>()};
  deckProps->width = 640;
  deckProps->height = 480;

  auto deck = std::make_shared<Deck>(deckProps);
  deck->run();

  return 0;
}
