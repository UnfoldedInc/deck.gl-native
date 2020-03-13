#include "./web-mercator-utils.h"

using namespace mathgl;

ViewMatrixOptions::ViewMatrixOptions() : viewMatrix{Matrix4d() /*identity*/}, focalDistance{1} {}

ProjectionMatrixOptions::ProjectionMatrixOptions()
    : orthographic{false}, fovy{75}, near{0.1}, far{1000}, focalDistance{1} {}
