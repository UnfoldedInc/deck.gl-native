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

// Partial port of https://github.com/visgl/deck.gl/blob/master/examples/website/geojson

#include <arrow/filesystem/localfs.h>

#include "deck.gl/core.h"
#include "deck.gl/layers.h"
#include "loaders.gl/json.h"

using namespace deckgl;

loadersgl::JSONLoader jsonLoader;
auto fileSystem = std::make_shared<arrow::fs::LocalFileSystem>();

auto createViewState(double bearing) -> std::shared_ptr<ViewState> {
  auto viewState = std::make_shared<ViewState>();
  viewState->latitude = 49.24;
  viewState->longitude = -123.1;
  viewState->zoom = 10.6;
  viewState->pitch = 30.0;
  viewState->bearing = bearing;

  return viewState;
}

auto createSolidPolygonLayer(const std::string &dataPath) -> std::shared_ptr<SolidPolygonLayer::Props> {
  auto props = std::make_shared<SolidPolygonLayer::Props>();
  props->id = "blocks";
  props->data = jsonLoader.loadTable(fileSystem->OpenInputStream(dataPath).ValueOrDie());
  props->getPolygon = [](const Row &row) { return row.getVector3List<float>("coordinates"); };
  props->getFillColor = [](const Row &row) {
    return mathgl::Vector4<float>{255.0f, 0.0f, 0.0f, (row.getFloat("value") / 20000.0f) * 255.0f};
  };

  return props;
}

int main(int argc, const char *argv[]) {
  // Get data file paths relative to working directory
  auto programPath = std::string{argv[0]};
  auto programDirectory = programPath.erase(programPath.find_last_of("/"));
  auto vancouverDataPath = programDirectory + "/data/vancouver-blocks-simplified.ndjson";

  auto deckProps = std::make_shared<Deck::Props>();
  deckProps->id = "Vancouver Blocks";
  deckProps->layers = {createSolidPolygonLayer(vancouverDataPath)};
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
