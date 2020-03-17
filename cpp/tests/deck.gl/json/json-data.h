static auto jsonDataFull = R"JSON(
{
  "@@type": "Deck",
  "description": "Test",
  "initialViewState": {
    "longitude": -122.45,
    "latitude": 37.8,
    "zoom": 12
  },
  "layers": [
    {
      "@@type": "ScatterplotLayer",
      "data": [
        {
          "position": [
            -122.45,
            37.8
          ]
        }
      ],
      "getFillColor": [
        255,
        0,
        0,
        255
      ],
      "getRadius": 1000
    },
    {
      "@@type": "TextLayer",
      "data": [
        {
          "position": [
            -122.45,
            37.8
          ],
          "text": "Hello World"
        }
      ],
      "getTextAnchor": "end"
    },
    {
      "@@type": "GeoJsonLayer",
      "data": {
        "type": "FeatureCollection",
        "features": [
          {
            "type": "Feature",
            "properties": {},
            "geometry": {
              "type": "Point",
              "coordinates": [
                -122.42923736572264,
                37.80544394934271
              ]
            }
          }
        ]
      },
      "stroked": true,
      "filled": true,
      "lineWidthMinPixels": 2,
      "opacity": 0.4,
      "getLineColor": [
        255,
        100,
        100
      ],
      "getFillColor": [
        200,
        160,
        0,
        180
      ]
    },
    {
      "@@type": "PointCloudLayer",
      "data": [
        {
	  "position": [-122.4, 37.7, 12],
	  "normal": [-1, 0, 0],
	  "color": [255, 255, 0]
	}
      ],
      "pointSize": 10,
      "coordinateSystem": "@@#COORDINATE_SYSTEM.METER_OFFSETS",
      "coordinateOrigin": [-122.4, 37.74]
    }
  ]
}
)JSON";

static auto jsonDataSimple = R"JSON(
{
  "@@type": "Deck",
  "description": "Test",
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
