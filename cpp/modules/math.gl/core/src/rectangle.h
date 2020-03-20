
#ifndef MATHGL_CORE_RECTANGLE_H
#define MATHGL_CORE_RECTANGLE_H

#include <ostream>

namespace mathgl {

template <typename coord>
class Rectangle {
 public:
  Rectangle() : x(0), y(0), w(0), h(0) {}
  Rectangle(coord x1, coord y1, coord w1, coord h1) : x(x1), y(y1), w(w1), h(h1) {}

  auto operator==(const Rectangle<coord> &r) const -> bool { return x == r.x && y == r.y && w == r.w && h == r.h; }

  coord x, y, w, h;
};

template <typename coord>
auto operator<<(std::ostream &os, const Rectangle<coord> &r) -> std::ostream & {
  os << "Rect(x=" << r.x << ", y=" << r.y << ", width=" << r.w << ", height=" << r.h << ")";
  return os;
}

}  // namespace mathgl

#endif
