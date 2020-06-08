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

#ifndef DECKGL_CORE_CONSTANTS_H
#define DECKGL_CORE_CONSTANTS_H

#include "deck.gl/json.h"

namespace deckgl {

// NOTE: The numeric values here are matched by shader code in the
// "project" and "project64" shader modules. Both places need to be updated.

/// \brief Describes how positions are interpreted. Can be specified per layer
enum class COORDINATE_SYSTEM {
  DEFAULT = -1,  // `LNGLAT` if rendering into a geospatial viewport,
                 // `CARTESIAN` otherwise

  // Non-geospatial coordinates
  CARTESIAN = 0,

  // Geospatial coordinates
  LNGLAT = 1,          // Positions interpreted as [lng, lat, elevation]. lng + lat in
                       // degrees, elevation + distances in meters.
  METER_OFFSETS = 2,   // Positions are interpreted as meter offsets, distances as meters
  LNGLAT_OFFSETS = 3,  // Positions are lng lat offsets: [deltaLng, deltaLat,
                       // elevation]. elevation+distances are meters.
};

auto operator<<(std::ostream& os, COORDINATE_SYSTEM cs) -> std::ostream&;

// TODO(ib@unfolded.ai): Decide how to deserialize enum constants
template <>
inline auto fromJson<COORDINATE_SYSTEM>(const Json::Value& jsonValue) -> COORDINATE_SYSTEM {
  return static_cast<COORDINATE_SYSTEM>(fromJson<int>(jsonValue));
}

/// \brief Describes the common space.
enum class PROJECTION_MODE {
  IDENTITY = 0,
  WEB_MERCATOR = 1,
  WEB_MERCATOR_AUTO_OFFSET = 4  // This is automatically assigned by the project module
};

}  // namespace deckgl

#endif  // DECKGL_CORE_CONSTANTS_H
