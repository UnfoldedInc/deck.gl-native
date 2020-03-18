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
    mathgl::Vector3<double> addMetersToLngLatResults[4];
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
    600,
    {{-122.42988626112113,37.75008993198172,10},
    {-122.42886259877028,37.7508993148988,100},
    {-122.41862474343641,37.75899265705196,1000},
    {-122.37309604668772,37.794952343211726,5000}}
  },
   { //pitched
    37.75,
    -122.43,
    11.5,
    30,
    0,
    0,
    800,
    600,
    {{-122.42988626112113,37.75008993198172,10},
    {-122.42886259877028,37.7508993148988,100},
    {-122.41862474343641,37.75899265705196,1000},
    {-122.37309604668772,37.794952343211726,5000}}
  },
  { // rotated
    37.7749,
    -122.4194,
    11,
    60,
    180,
    1.5,
    1267,
    400,
    {{-122.41928622282498,37.77498993198168,10},
    {-122.41826221579376,37.775799314893895,100},
    {-122.40802091213472,37.78389265656107,1000},
    {-122.36247685600216,37.819852330927304,5000}}
  },
  { // highLatitude
    75.751537,
    42.42694,
    15.5,
    20,
    -40,
    1.5,
    500,
    500,
    {
      {42.42730539057204,75.75162693175842,10},
      {42.4305941089934,75.75243629256823,100},
      {42.46350142579452,75.76052742250958,1000},
      {42.61019997853004,75.7964333140059,5000}
    }
  }
};

#endif
