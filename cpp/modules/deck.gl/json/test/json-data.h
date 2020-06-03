// Copyright (c) 2020, Unfolded Inc
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

static auto jsonDataFull = R"JSON(
{
  "@@type": "Deck",
  "id": "TestDeck",
  "width": 640,
  "height": 480,
  "initialViewState": {
    "longitude": -122.45,
    "latitude": 37.8,
    "zoom": 12,
    "bearing": 20,
    "pitch": 30
  },
  "views": [
    {
      "@@type": "MapView",
      "id": "FirstView",
      "x": 5,
      "y": 10,
      "width": 400,
      "height": 300,
      "fovy": 70.0,
      "near": 1.0,
      "far": 500.0
    },
    {
      "@@type": "View"
    }
  ],
  "layers": [
    {
      "@@type": "LineLayer",
      "id": "LineLayer",
      "data": [],
      "visible": false,
      "opacity": 0.5,
      "coordinateOrigin": [5.0, 3.0, 2.0],
      "coordinateSystem": 1,
      "wrapLongitude": true,
      "positionFormat": "YZX",
      "colorFormat": "BGRA",
      "widthUnits": "meters",
      "widthScale": 2.0,
      "widthMinPixels": 1.0,
      "widthMaxPixels": 100.0
    },
    {
      "@@type": "ScatterplotLayer",
      "filled": false,
      "stroked": true,
      "lineWidthUnits": "pixels",
      "lineWidthScale": 2,
      "lineWidthMinPixels": 1,
      "lineWidthMaxPixels": 5.2,
      "radiusScale": 3,
      "radiusMinPixels": 1,
      "radiusMaxPixels": 10
    },
    {
      "@@type": "SolidPolygonLayer",
      "filled": false,
      "elevationScale": 2
    }
  ]
}
)JSON";

static auto jsonDataSimple = R"JSON(
{
  "@@type": "Deck",
  "id": "TestDeck",
  "initialViewState": {
    "longitude": -122.45,
    "latitude": 37.8,
    "zoom": 12
  },
  "layers": [
    {
      "@@type": "ScatterplotLayer",
      "data": [
          [
            -122.45,
            37.8
          ]
      ],
      "getRadius": 1000
    },
    {
      "@@type": "LineLayer",
      "data": [
      ]
    }
  ]
}
)JSON";
