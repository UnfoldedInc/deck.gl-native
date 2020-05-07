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

auto createViewState(double bearing) -> std::shared_ptr<ViewState> {
  auto viewState = std::make_shared<ViewState>();
  viewState->latitude = 47.65;
  viewState->longitude = 7.0;
  viewState->zoom = 4.5;
  viewState->pitch = 50.0;
  viewState->bearing = bearing;

  return viewState;
}

auto createLineLayer(const std::string &dataPath) -> std::shared_ptr<LineLayer::Props> {
  auto props = std::make_shared<LineLayer::Props>();
  props->id = "flight-paths";
  props->opacity = 0.8f;
  props->getSourcePosition = [](const Row &row) { return row.getVector3<float>("start"); };
  props->getTargetPosition = [](const Row &row) { return row.getVector3<float>("end"); };
  props->getColor = [](const Row &row) -> mathgl::Vector4<float> {
    float r = row.getVector3<float>("start").z / 10000.0f;
    return {255.0f * (1.0f - r * 2.0f), 128.0f * r, 255.0f * r, 255.0f * (1.0f - r)};
  };
  props->getWidth = [](const Row &row) { return 3.0f; };
  props->data = jsonLoader.loadTable(fileSystem->OpenInputStream(dataPath).ValueOrDie());

  return props;
}

auto createScatterplotLayer(const std::string &dataPath) -> std::shared_ptr<ScatterplotLayer::Props> {
  auto props = std::make_shared<ScatterplotLayer::Props>();
  props->id = "airports";
  props->getPosition = [](const Row &row) { return row.getVector3<float>("coordinates"); };
  props->radiusScale = 40.0f;
  props->getRadius = [](const Row &row) -> float {
    auto type = row.getString("type");
    if (type == "major") {
      return 100.0f;
    } else if (type == "small") {
      return 30.0f;
    } else {
      return 60.0f;
    }
  };
  props->getFillColor = [](const Row &row) { return mathgl::Vector4<float>{255.0f, 144.0f, 0.0f, 255.0f}; };
  props->stroked = true;
  props->getLineWidth = [](const Row &row) { return 5.0f; };
  props->getLineColor = [](const Row &row) { return mathgl::Vector4<float>{255.0f, 0.0f, 0.0f, 255.0f}; };

  props->data = jsonLoader.loadTable(fileSystem->OpenInputStream(dataPath).ValueOrDie());

  return props;
}

int main(int argc, const char *argv[]) {
  // Get data file paths relative to working directory
  auto programPath = std::string{argv[0]};
  auto programDirectory = programPath.erase(programPath.find_last_of("/"));
  auto flightDataPath = programDirectory + "/data/heathrow-flights.ndjson";
  auto airportDataPath = programDirectory + "/data/airports.ndjson";

  auto deckProps = std::make_shared<Deck::Props>();
  deckProps->id = "Heathrow Flights";
  deckProps->layers = {createLineLayer(flightDataPath), createScatterplotLayer(airportDataPath)};
  deckProps->initialViewState = createViewState(0.0);
  deckProps->views = {std::make_shared<MapView>()};
  deckProps->width = 640;
  deckProps->height = 480;

  auto deck = std::make_shared<Deck>(deckProps);
  deck->run([&deckProps](Deck *deck) {
    static double bearing = 0.0;
    deckProps->viewState = createViewState(bearing += 0.3);
    deck->setProps(deckProps);
  });

  return 0;
}
