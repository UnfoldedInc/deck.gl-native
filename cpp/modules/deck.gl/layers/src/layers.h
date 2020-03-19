// Copyright (c) 2020 Unfolded, Inc.
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

#ifndef DECKGL_LAYERS_LAYERS_H
#define DECKGL_LAYERS_LAYERS_H

// To be included in prototype
#include "./line-layer/line-layer.h"                // {LineLayer}
#include "./scatterplot-layer/scatterplot-layer.h"  // {ScatterplotLayer}
// #include "./solid-polygon-layer/solid-polygon-layer.h" // SolidPolygonLayer

// Not to be included prototype
/*
#include './arc-layer/arc-layer'; // ArcLayer
#include './bitmap-layer/bitmap-layer'; // BitmapLayer
#include './icon-layer/icon-layer'; // IconLayer
#include './line-layer/line-layer'; // LineLayer
#include './point-cloud-layer/point-cloud-layer'; // PointCloudLayer
#include './scatterplot-layer/scatterplot-layer'; // ScatterplotLayer
#include './column-layer/column-layer'; // ColumnLayer
#include './column-layer/grid-cell-layer'; // GridCellLayer
#include './path-layer/path-layer'; // PathLayer
#include './polygon-layer/polygon-layer'; // PolygonLayer
#include './geojson-layer/geojson-layer'; // GeoJsonLayer
#include './text-layer/text-layer'; // TextLayer
#include './solid-polygon-layer/solid-polygon-layer'; // SolidPolygonLayer
#include './base-tile-layer/base-tile-layer'; // BaseTileLayer
*/

namespace deckgl {

void registerJSONConvertersForDeckLayers(JSONConverter *);

}  // namespace deckgl

#endif  // DECKGL_LAYERS_LAYERS_H
