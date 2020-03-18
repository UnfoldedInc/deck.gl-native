#ifndef MATHGL_TEST_WEB_MERCATOR_SAMPLE_VIEWPORTS_H
#define MATHGL_TEST_WEB_MERCATOR_SAMPLE_VIEWPORTS_H

struct SampleViewport {
    double latitude;
    double longitude;
    double zoom;
    double pitch;
    double bearing;
    double altitude;
    double width;
    double height;
};

static const SampleViewport SAMPLE_VIEWPORTS[] = {
    { // flat
    37.75,
    -122.43,
    11.5,
    0,
    0,
    0,
    800,
    600
  },
   { //pitched
    37.75,
    -122.43,
    11.5,
    30,
    0,
    0,
    800,
    600
  },
  { // rotated
    37.7749,
    -122.4194,
    11,
    60,
    180,
    1.5,
    1267,
    400
  },
  { // highLatitude
    75.751537,
    42.42694,
    15.5,
    20,
    -40,
    1.5,
    500,
    500
  }
};

#endif
