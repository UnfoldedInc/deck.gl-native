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

#ifndef MATHGL_CORE_CORE_H
#define MATHGL_CORE_CORE_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <ostream>

#undef PI  // Get rid of nasty cmath macro (conflicts e.g. with ranges)

namespace mathgl {

// Approx equals for floating point
template <typename T>
inline bool equalsT(const T &a, const T &b) {
  return a == b;
}
template <>
inline bool equalsT<float>(const float &a, const float &b) {
  return std::abs(a - b) < std::numeric_limits<float>::epsilon();
}
template <>
inline bool equalsT<double>(const double &a, const double &b) {
  return std::abs(a - b) < std::numeric_limits<float>::epsilon();
}

// Templates
template <typename coord>
class Vector2;
template <typename coord>
class Vector3;
template <typename coord>
class Vector4;

template <typename coord>
class Matrix2;
template <typename coord>
class Matrix3;
template <typename coord>
class Matrix4;

///////////////////////////////////////////////////////////
//  Vector2

template <typename coord>
class Vector2 {
 public:
  Vector2() : x(static_cast<coord>(0)), y(static_cast<coord>(0)) {}
  Vector2(coord x, coord y) : x(x), y(y) {}
  template <typename othercoord>
  explicit Vector2(const Vector2<othercoord> &other);

  auto operator-() const -> Vector2<coord> { return Vector2<coord>{-x, -y}; }
  auto operator+=(const Vector2<coord> &v) -> Vector2<coord> {
    x += v.x, y += v.y;
    return *this;
  }
  auto operator-=(const Vector2<coord> &v) -> Vector2<coord> {
    x -= v.x, y -= v.y;
    return *this;
  }
  auto operator*=(coord a) -> Vector2<coord> {
    x *= a, y *= a;
    return *this;
  }
  auto operator/=(coord a) -> Vector2<coord> {
    x /= a, y /= a;
    return *this;
  }

  coord length() const;
  coord length2() const;
  coord angle() const;

  void normalize();

  auto lerp(const Vector2<coord> &, coord t) -> Vector2<coord>;

  coord x, y;
};

template <typename coord>
auto dotProduct(const Vector2<coord> &, const Vector2<coord> &) -> Vector2<coord>;

template <typename coord>
auto operator+(const Vector2<coord> &v1, const Vector2<coord> &v2) -> Vector2<coord> {
  return Vector2<coord>{v1.x + v2.x, v1.y + v2.y};
}

template <typename coord>
auto operator-(const Vector2<coord> &v1, const Vector2<coord> &v2) -> Vector2<coord> {
  return Vector2<coord>{v1.x - v2.x, v1.y - v2.y};
}

template <typename coord>
auto operator==(const Vector2<coord> &v1, const Vector2<coord> &v2) -> bool {
  return equalsT(v1.x, v2.x) && equalsT(v1.y, v2.y);
}

template <typename coord>
auto operator!=(const Vector2<coord> &v1, const Vector2<coord> &v2) -> bool {
  return !(v1 == v2);
}

///////////////////////////////////////////////////////////
//  Vector3

template <typename coord>
class Vector3 {
 public:
  Vector3() : x(0), y(0), z(0) {}
  Vector3(coord x1, coord y1, coord z1) : x(x1), y(y1), z(z1) {}
  Vector3(Vector2<coord> xy, coord z1) : x(xy.x), y(xy.y), z(z1) {}
  template <typename othercoord>
  explicit Vector3(const Vector3<othercoord> &other);

  auto operator-() const -> Vector3<coord> { return Vector3<coord>{-x, -y, -z}; }
  auto operator+=(const Vector3<coord> &v) -> Vector3<coord> {
    x += v.x, y += v.y, z += v.z;
    return *this;
  }
  auto operator-=(const Vector3<coord> &v) -> Vector3<coord> {
    x -= v.x, y -= v.y, z -= v.z;
    return *this;
  }
  auto operator*=(coord a) -> Vector3<coord> {
    x *= a, y *= a, z *= a;
    return *this;
  }
  auto operator/=(coord a) -> Vector3<coord> {
    x /= a, y /= a, z /= a;
    return *this;
  }

  auto length() const -> coord;
  auto length2() const -> coord;
  auto toVector2() const -> Vector2<coord>;

  void normalize();

  coord x, y, z;
};

template <typename coord>
auto crossProduct(const Vector3<coord> &, const Vector3<coord> &) -> Vector3<coord>;

template <typename coord>
auto dotProduct(const Vector3<coord> &, const Vector3<coord> &) -> Vector3<coord>;

template <typename coord>
auto operator+(const Vector3<coord> &v1, const Vector3<coord> &v2) -> Vector3<coord> {
  return Vector3<coord>{v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

template <typename coord>
auto operator-(const Vector3<coord> &v1, const Vector3<coord> &v2) -> Vector3<coord> {
  return Vector3<coord>{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
}

template <typename coord>
auto operator==(const Vector3<coord> &v1, const Vector3<coord> &v2) -> bool {
  return equalsT(v1.x, v2.x) && equalsT(v1.y, v2.y) && equalsT(v1.z, v2.z);
}

template <typename coord>
auto operator!=(const Vector3<coord> &v1, const Vector3<coord> &v2) -> bool {
  return !(v1 == v2);
}

///////////////////////////////////////////////////////////
//  Vector4

template <typename coord>
class Vector4 {
 public:
  Vector4() : x(0), y(0), z(0), w(0) {}
  Vector4(coord x1, coord y1, coord z1, coord w1) : x(x1), y(y1), z(z1), w(w1) {}
  Vector4(Vector3<coord> xyz, coord w1) : x(xyz.x), y(xyz.y), z(xyz.z), w(w1) {}
  template <typename othercoord>
  explicit Vector4(const Vector4<othercoord> &other);

  auto operator-() const -> Vector4<coord> { return Vector4<coord>{-x, -y, -z, -w}; }

  auto operator+=(const Vector4<coord> &v) -> Vector4<coord> {
    x += v.x, y += v.y, z += v.z, w += v.w;
    return *this;
  }
  auto operator-=(const Vector4<coord> &v) -> Vector4<coord> {
    x -= v.x, y -= v.y, z -= v.z, w -= v.w;
    return *this;
  }
  auto operator*=(coord a) -> Vector4<coord> {
    x *= a, y *= a, z *= a, w *= a;
    return *this;
  }
  auto operator/=(coord a) -> Vector4<coord> {
    x /= a, y /= a, z /= a, w /= a;
    return *this;
  }

  auto operator+(const Vector4<coord> &v) const -> Vector4<coord> {
    return Vector4<coord>{x + v.x, y + v.y, z + v.z, w + v.w};
  }

  auto operator-(const Vector4<coord> &v) const -> Vector4<coord> {
    return Vector4<coord>{x - v.x, y - v.y, z - v.z, w - v.w};
  }

  auto operator==(const Vector4<coord> &v) const -> bool {
    return equalsT(x, v.x) && equalsT(y, v.y) && equalsT(z, v.z) && equalsT(w, v.w);
  }

  auto transform(const Matrix4<coord> &m) const -> Vector4<coord>;

  // TODO(ilija@unfolded.ai): These are not implemented?
  coord length() const;
  coord length2() const;
  auto toVector3() const -> Vector3<coord>;

  void normalize();

  coord x, y, z, w;
};

template <typename coord>
template <typename othercoord>
Vector4<coord>::Vector4(const Vector4<othercoord> &other)
    : x{static_cast<coord>(other.x)},
      y{static_cast<coord>(other.y)},
      z{static_cast<coord>(other.z)},
      w{static_cast<coord>(other.w)} {}

template <typename coord>
Vector4<coord> dotProduct(const Vector4<coord> &, const Vector4<coord> &);

template <typename coord>
auto operator+(const Vector4<coord> &v1, const Vector4<coord> &v2) -> Vector4<coord> {
  return Vector4<coord>{v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w};
}

template <typename coord>
auto operator-(const Vector4<coord> &v1, const Vector4<coord> &v2) -> Vector4<coord> {
  return Vector4<coord>{v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w};
}

template <typename coord>
auto operator==(const Vector4<coord> &v1, const Vector4<coord> &v2) -> bool {
  return equalsT(v1.x, v2.x) && equalsT(v1.y, v2.y) && equalsT(v1.z, v2.z) && equalsT(v1.w, v2.w);
}

template <typename coord>
auto operator!=(const Vector4<coord> &v1, const Vector4<coord> &v2) -> bool {
  return !(v1 == v2);
}

///////////////////////////////////////////////////////////
//  Two-dimensional matrix support

template <typename coord>
class Matrix2 {
 public:
  // Template types
  typedef coord Coord;

  Matrix2(coord m11, coord m12, coord m21, coord m22);
  Matrix2(const Matrix2<coord> &);
  // Matrix2 (Matrix2<coord> &&) = default;
  // Matrix2 & operator= (const Matrix2<coord> &) = default;
  // Matrix2 & operator= (Matrix2<coord> &&) = default;

  static auto makeUnit() -> Matrix2<coord>;
  static auto makeScale(coord sx, coord sy) -> Matrix2;
  static auto makeRotation(coord angle) -> Matrix2;  // counterclockwise

  auto invert() const -> Matrix2<coord>;

  auto operator()(unsigned row, unsigned col) -> coord & { return _m[row][col]; }
  auto operator()(unsigned row, unsigned col) const -> const coord { return _m[row][col]; }

 private:
  coord _m[2][2];
  auto at(int row, int column) -> coord & { return (*this)(row, column); }
  auto at(int row, int column) const -> const coord { return (*this)(row, column); }
};

// Matrix operator+ (const Matrix &) const;
// Matrix operator- (const Matrix &) const;
// template <typename T>Matrix operator* (const Matrix &) const;
// Vector2 operator* (const Vector2 &) const;

///////////////////////////////////////////////////////////
//  Three-dimensional matrix support, 2D homogenous coordinates

template <typename coord>
class Matrix3 {
 public:
  Matrix3(coord a11, coord a12, coord a13, coord a21, coord a22, coord a23, coord a31, coord a32, coord a33);
  Matrix3(const Matrix3<coord> &);

  auto operator=(const Matrix3<coord> &) -> Matrix3<coord> &;
  auto operator=(Matrix3<coord> &&) -> Matrix3<coord> &;

  auto operator()(int row, int col) -> coord & { return _m[row][col]; }
  auto operator()(int row, int col) const -> const coord { return _m[row][col]; }

  // Matrix creation
  static auto makeUnit() -> Matrix3<coord>;
  static auto makeTranslation(const Vector2<coord> &offset) -> Matrix3<coord>;
  static auto makeScale(coord scaleX, coord scaleY) -> Matrix3<coord>;
  static auto makeRotation(coord angle) -> Matrix3<coord>;
  static auto makeShear(coord factorX, coord factorY) -> Matrix3<coord>;

  // Matrix Algebra
  auto determinant() const -> coord;
  auto invert() const -> Matrix3<coord>;
  auto transpose() const -> Matrix3<coord>;

  // Vector multiplication
  // TODO(randy@unfolded.ai) multiply
  auto multiply(const Vector3<coord>) const -> Vector3<coord>;

  // Homogenous coordinates
  auto isHomogeneous() const -> bool;
  auto multiplyPoint(const Vector2<coord>) const -> Vector2<coord>;
  // Commented out?
  auto multiplyVector(const Vector2<coord>) const -> Vector2<coord>;

 private:
  coord _m[3][3];
  auto at(int row, int column) -> coord & { return (*this)(row, column); }
  auto at(int row, int column) const -> const coord { return (*this)(row, column); }
};

template <class T>
auto operator+(const Matrix3<T> &, const Matrix3<T> &) -> Matrix3<T>;
template <class T>
auto operator-(const Matrix3<T> &, const Matrix3<T> &) -> Matrix3<T>;
template <class T>
auto operator*(const Matrix3<T> &, const Matrix3<T> &) -> Matrix3<T>;

template <typename coord>
auto operator>>(std::istream &is, Matrix3<coord> &m) -> std::istream & {
  is >> m(0, 0) >> m(0, 1) >> m(0, 2) >> m(1, 0) >> m(1, 1) >> m(1, 2) >> m(2, 0) >> m(2, 1) >> m(2, 2);
  return is;
}

template <typename coord>
auto operator<<(std::ostream &os, const Matrix3<coord> &m) -> std::ostream & {
  os << "[[" << m(0, 0) << " " << m(0, 1) << " " << m(0, 2) << "]"
     << " [" << m(1, 0) << " " << m(1, 1) << " " << m(1, 2) << "]"
     << " [" << m(2, 0) << " " << m(2, 1) << " " << m(2, 2) << "]]";
  return os;
}

///////////////////////////////////////////////////////////
// Four-dimensional matrix support, 3D homogenous coordinates

template <typename coord>
class Matrix4 {
 public:
  // Template types
  typedef coord Coord;

  Matrix4();
  Matrix4(coord m11, coord m12, coord m13, coord m14, coord m21, coord m22, coord m23, coord m24, coord m31, coord m32,
          coord m33, coord m34, coord m41, coord m42, coord m43, coord m44);
  Matrix4(const Matrix4<coord> &) = default;
  Matrix4(Matrix4<coord> &&) = default;
  template <typename othercoord>
  explicit Matrix4(const Matrix4<othercoord> &other);

  auto operator=(const Matrix4<coord> &) -> Matrix4<coord> & = default;
  auto operator=(Matrix4<coord> &&) -> Matrix4<coord> & = default;

  // Matrix Creation
  static auto makeUnit() -> Matrix4<coord>;
  static auto makeTranslation(const Vector3<coord> &offset) -> Matrix4<coord>;
  static auto makeScale(const Vector3<coord> &scale) -> Matrix4<coord>;
  static auto makeRotationX(coord angle) -> Matrix4<coord>;
  static auto makeRotationY(coord angle) -> Matrix4<coord>;
  static auto makeRotationZ(coord angle) -> Matrix4<coord>;
  // Commented out
  static auto makeShearXY(coord factorX, coord factorY) -> Matrix4<coord>;
  static auto makeShearXZ(coord factorX, coord factorZ) -> Matrix4<coord>;
  // TODO(randy@unfolded.ai)
  static auto makeShearYX(coord factorY, coord factorZ) -> Matrix4<coord>;
  // TODO(randy@unfolded.ai)
  // can't find one that just takes a distance parameter, maybe just use orthogonal?
  static auto makeProjection(coord distance) -> Matrix4<coord>;
  // TODO(randy@unfolded.ai)
  static auto makePerspective(coord fovy, coord aspect, coord near, coord far) -> Matrix4<coord>;

  auto operator()(int row, int col) -> coord & { return _m[row][col]; }
  auto operator()(int row, int col) const -> const coord { return _m[row][col]; }

  auto determinant() const -> coord;
  // TODO(randy@unfolded.ai)
  auto invert() const -> Matrix4<coord>;
  auto transpose() const -> Matrix4<coord>;

  bool isHomogeneous() const;
  // incorrect current implementation for multiplyVector/multiplyPoint
  auto multiplyVector(const Vector3<coord> &) const -> Vector3<coord>;
  auto multiplyPoint(const Vector3<coord> &) const -> Vector3<coord>;

  // TODO(randy@unfolded.ai) revisit
  auto scale(const Vector3<coord> &) const -> Matrix4<coord>;
  auto translate(const Vector3<coord> &t) const -> Matrix4<coord>;
  auto rotateX(const coord rad) -> Matrix4<coord>;
  auto rotateY(const coord rad) -> Matrix4<coord>;
  auto rotateZ(const coord rad) -> Matrix4<coord>;

 private:
  coord _m[4][4];
  auto at(int row, int column) -> coord & { return (*this)(row, column); }
  auto at(int row, int column) const -> const coord { return (*this)(row, column); }
};

template <typename coord>
auto operator>>(std::istream &is, Matrix4<coord> &m) -> std::istream & {
  is >> m(0, 0) >> m(0, 1) >> m(0, 2) >> m(0, 3) >> m(1, 0) >> m(1, 1) >> m(1, 2) >> m(1, 3) >> m(2, 0) >> m(2, 1) >>
      m(2, 2) >> m(2, 3) >> m(3, 0) >> m(3, 1) >> m(3, 2) >> m(3, 3);
  return is;
}

template <typename coord>
auto operator<<(std::ostream &os, const Matrix4<coord> &m) -> std::ostream & {
  os << "[[" << m(0, 0) << " " << m(0, 1) << " " << m(0, 2) << " " << m(0, 3) << "]"
     << " [" << m(1, 0) << " " << m(1, 1) << " " << m(1, 2) << " " << m(1, 3) << "]"
     << " [" << m(2, 0) << " " << m(2, 1) << " " << m(2, 2) << " " << m(2, 3) << "]"
     << " [" << m(3, 0) << " " << m(3, 1) << " " << m(3, 2) << " " << m(3, 3) << "]]";
  return os;
}

template <typename coord>
auto operator==(const Matrix4<coord> &l, const Matrix4<coord> &r) -> bool {
  return equalsT(l(0, 0), r(0, 0)) && equalsT(l(0, 1), r(0, 1)) && equalsT(l(0, 2), r(0, 2)) &&
         equalsT(l(0, 3), r(0, 3)) && equalsT(l(1, 0), r(1, 0)) && equalsT(l(1, 1), r(1, 1)) &&
         equalsT(l(1, 2), r(1, 2)) && equalsT(l(1, 3), r(1, 3)) && equalsT(l(2, 0), r(2, 0)) &&
         equalsT(l(2, 1), r(2, 1)) && equalsT(l(2, 2), r(2, 2)) && equalsT(l(2, 3), r(2, 3)) &&
         equalsT(l(3, 0), r(3, 0)) && equalsT(l(3, 1), r(3, 1)) && equalsT(l(3, 2), r(3, 2)) &&
         equalsT(l(3, 3), r(3, 3));
}

template <typename coord>
auto operator!=(const Matrix4<coord> &v1, const Matrix4<coord> &v2) -> bool {
  return !(v1 == v2);
}

///////////////////////////////////////////////////////////
// Vector2 implementation

template <typename coord>
template <typename othercoord>
Vector2<coord>::Vector2(const Vector2<othercoord> &other)
    : x{static_cast<coord>(other.x)}, y{static_cast<coord>(other.y)} {}

template <typename coord>
auto Vector2<coord>::length() const -> coord {
  return static_cast<coord>(std::sqrt(x * x + y * y));
}

template <typename coord>
auto Vector2<coord>::length2() const -> coord {
  return static_cast<coord>(x * x + y * y);
}

template <typename coord>
auto Vector2<coord>::angle() const -> coord {
  if (x == static_cast<coord>(0) && y == static_cast<coord>(0)) return static_cast<coord>(0);
  return static_cast<coord>(atan2(y, x));
}

template <typename coord>
void Vector2<coord>::normalize() {
  coord length = length();
  if (length <= static_cast<coord>(0)) throw std::logic_error("normalize called on zero length vector");
  x /= length;
  y /= length;
}

template <typename coord>
auto Vector2<coord>::lerp(const Vector2<coord> &v, coord t) -> Vector2<coord> {
  return Vector2<coord>{x + t * (v.x - x), y + t * (v.y - y)};
}

template <typename coord>
auto dotProduct(const Vector2<coord> &u, const Vector2<coord> &v) -> Vector2<coord> {
  return Vector2<coord>{u.x * v.x, u.y * v.y};
}

template <typename coord>
auto operator*(const Vector2<coord> &u, const Vector2<coord> &v) -> coord {
  return u.x * v.x + u.y * v.y;
}

template <typename coord>
auto operator*(const Vector2<coord> &v, coord c) -> Vector2<coord> {
  return Vector2<coord>{v.x * c, v.y * c};
}

template <typename coord>
auto operator*(coord c, const Vector2<coord> &v) -> Vector2<coord> {
  return Vector2<coord>{v.x * c, v.y * c};
}

template <typename coord>
auto operator/(const Vector2<coord> &v, coord c) -> Vector2<coord> {
  return Vector2<coord>{v.x / c, v.y / c};
}

template <typename coord>
auto operator<<(std::ostream &os, const Vector2<coord> &v) -> std::ostream & {
  os << "(" << v.x << "," << v.y << ")";
  return os;
}

///////////////////////////////////////////////////////////
// Vector3 implementation

template <typename coord>
template <typename othercoord>
Vector3<coord>::Vector3(const Vector3<othercoord> &other)
    : x{static_cast<coord>(other.x)}, y{static_cast<coord>(other.y)}, z{static_cast<coord>(other.z)} {}

template <typename coord>
auto Vector3<coord>::length() const -> coord {
  return static_cast<coord>(sqrt(x * x + y * y + z * z));
}

template <typename coord>
auto Vector3<coord>::length2() const -> coord {
  return static_cast<coord>(x * x + y * y + z * z);
}

template <typename coord>
auto Vector3<coord>::toVector2() const -> Vector2<coord> {
  return Vector2<coord>{this->x, this->y};
}

template <typename coord>
void Vector3<coord>::normalize() {
  coord length = length();
  if (length <= static_cast<coord>(0)) throw std::runtime_error("normalize called on zero length vector");
  x /= length;
  y /= length;
  z /= length;
}

template <typename coord>
auto crossProduct(const Vector3<coord> &u, const Vector3<coord> &v) -> Vector3<coord> {
  return Vector3<coord>{u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x};
}

template <typename coord>
auto dotProduct(const Vector3<coord> &u, const Vector3<coord> &v) -> Vector3<coord> {
  return Vector3<coord>{u.x * v.x, u.y * v.y, u.z * v.z};
}

template <typename coord>
auto operator*(const Vector3<coord> &u, const Vector3<coord> &v) -> coord {
  return u.x * v.x + u.y * v.y + u.z * v.z;
}

template <typename coord>
auto operator*(const Vector3<coord> &v, coord c) -> Vector3<coord> {
  return Vector3<coord>{v.x * c, v.y * c, v.z * c};
}

template <typename coord>
auto operator*(coord c, const Vector3<coord> &v) -> Vector3<coord> {
  return Vector3<coord>{v.x * c, v.y * c, v.z * c};
}

template <typename coord>
auto operator/(const Vector3<coord> &v, coord c) -> Vector3<coord> {
  return Vector3<coord>{v.x / c, v.y / c, v.z / c};
}

template <typename coord>
auto operator<<(std::ostream &os, const Vector3<coord> &v) -> std::ostream & {
  os << "(" << v.x << "," << v.y << "," << v.z << ")";
  return os;
}

////////////////////////////
// Vector4 implementation //
////////////////////////////

template <typename coord>
auto Vector4<coord>::length() const -> coord {
  return static_cast<coord>(sqrt(x * x + y * y + z * z + w * w));
}

template <typename coord>
auto Vector4<coord>::length2() const -> coord {
  return static_cast<coord>(x * x + y * y + z * z + w * w);
}

template <typename coord>
auto Vector4<coord>::toVector3() const -> Vector3<coord> {
  return Vector3<coord>{this->x, this->y, this->z};
}

template <typename coord>
auto Vector4<coord>::transform(const Matrix4<coord> &m) const -> Vector4<coord> {
  return Vector4<coord>{
      m(0, 0) * x + m(0, 1) * y + m(0, 2) * z + m(0, 3) * w, m(1, 0) * x + m(1, 1) * y + m(1, 2) * z + m(1, 3) * w,
      m(2, 0) * x + m(2, 1) * y + m(2, 2) * z + m(2, 3) * w, m(3, 0) * x + m(3, 1) * y + m(3, 2) * z + m(3, 3) * w};
}

template <typename coord>
void Vector4<coord>::normalize() {
  coord length = length();
  if (length <= static_cast<coord>(0)) throw std::runtime_error("normalize called on zero length vector");
  x /= length;
  y /= length;
  z /= length;
  w /= length;
}

template <typename coord>
auto dotProduct(const Vector4<coord> &u, const Vector4<coord> &v) -> Vector4<coord> {
  return Vector4<coord>{u.x * v.x, u.y * v.y, u.z * v.z, u.w * v.w};
}

template <typename coord>
auto operator*(const Vector4<coord> &u, const Vector4<coord> &v) -> coord {
  return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
}

template <typename coord>
auto operator*(const Vector4<coord> &v, coord c) -> Vector4<coord> {
  return Vector4<coord>{v.x * c, v.y * c, v.z * c, v.w * c};
}

template <typename coord>
auto operator*(coord c, const Vector4<coord> &v) -> Vector4<coord> {
  return Vector4<coord>{v.x * c, v.y * c, v.z * c, v.w * c};
}

template <typename coord>
auto operator/(const Vector4<coord> &v, coord c) -> Vector4<coord> {
  return Vector4<coord>{v.x / c, v.y / c, v.z / c, v.w / c};
}

template <typename coord>
auto operator<<(std::ostream &os, const Vector4<coord> &v) -> std::ostream & {
  os << "(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
  return os;
}

///////////////////////////////////////////////////////////
// Matrix2 implementation

template <typename coord>
Matrix2<coord>::Matrix2(coord a11, coord a12, coord a21, coord a22) {
  at(0, 0) = a11;
  at(0, 1) = a12;
  at(1, 0) = a21;
  at(1, 1) = a22;
}

template <typename coord>
Matrix2<coord>::Matrix2(const Matrix2<coord> &m2) {
  at(0, 0) = m2(0, 0);
  at(0, 1) = m2(0, 1);
  at(1, 0) = m2(1, 0);
  at(1, 1) = m2(1, 1);
}

template <typename coord>
auto Matrix2<coord>::makeUnit() -> Matrix2<coord> {
  return Matrix2(1, 0, 0, 1);
}

template <typename coord>
auto Matrix2<coord>::makeScale(coord sx, coord sy) -> Matrix2<coord> {
  return Matrix2(sx, static_cast<coord>(0), static_cast<coord>(0), sy);
}

template <typename coord>
auto Matrix2<coord>::makeRotation(coord angle) -> Matrix2<coord> {
  coord sine = static_cast<coord>(sin(angle));
  coord cosine = static_cast<coord>(cos(angle));  // ERROR adapt function to precision?
  return Matrix2{cosine, -sine, sine, cosine};
}

template <typename coord>
auto Matrix2<coord>::invert() const -> Matrix2<coord> {
  // Cramers rule
  coord det = at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0);
  if (det == static_cast<coord>(0)) throw std::runtime_error("Attempt to invert singular matrix");
  det = 1 / det;
  return Matrix2<coord>{det * at(1, 1), -det * at(0, 1), -det * at(1, 0), det * at(0, 0)};
}

template <typename coord>
auto operator*(const Matrix2<coord> &m1, const Matrix2<coord> &m2) -> Matrix2<coord> {
  // Unrolled multiply
  return Matrix2<coord>{m1(0, 0) * m2(0, 0) + m1(0, 1) * m2(1, 0), m1(0, 0) * m2(1, 0) + m1(0, 1) * m2(1, 1),
                        m1(0, 1) * m2(0, 0) + m1(1, 1) * m2(1, 0), m1(0, 1) * m2(1, 0) + m1(1, 1) * m2(1, 1)};
}

template <typename coord>
auto operator*(const Matrix2<coord> &m, const Vector2<coord> &v) -> Vector2<coord> {
  return Vector2<coord>{m(0, 0) * v.x + m(0, 1) * v.y, m(1, 0) * v.x + m(1, 1) * v.y};
}

template <typename coord>
auto operator>>(std::istream &is, Matrix2<coord> &m) -> std::istream & {
  is >> m(0, 0) >> m(0, 1) >> m(1, 0) >> m(1, 1);
  return is;
}

template <typename coord>
auto operator<<(std::ostream &os, const Matrix2<coord> &m) -> std::ostream & {
  os << "[[" << m(0, 0) << " " << m(0, 1) << "]"
     << " [" << m(1, 0) << " " << m(1, 1) << "]]";
  return os;
}

///////////////////////////////////////////////////////////
// Matrix3 implementation

template <typename coord>
Matrix3<coord>::Matrix3(coord a11, coord a12, coord a13, coord a21, coord a22, coord a23, coord a31, coord a32,
                        coord a33) {
  at(0, 0) = a11;
  at(0, 1) = a12;
  at(0, 2) = a13;
  at(1, 0) = a21;
  at(1, 1) = a22;
  at(1, 2) = a23;
  at(2, 0) = a31;
  at(2, 1) = a32;
  at(2, 2) = a33;
}

template <typename coord>
Matrix3<coord>::Matrix3(const Matrix3<coord> &m2) {
  at(0, 0) = m2(0, 0);
  at(0, 1) = m2(0, 1);
  at(0, 2) = m2(0, 2);
  at(1, 0) = m2(1, 0);
  at(1, 1) = m2(1, 1);
  at(1, 2) = m2(1, 2);
  at(2, 0) = m2(2, 0);
  at(2, 1) = m2(2, 1);
  at(2, 2) = m2(2, 2);
}

template <typename coord>
bool Matrix3<coord>::isHomogeneous() const {
  return at(2, 0) == 0 && at(2, 1) == 0 && at(3, 2) == 1;
}

template <typename coord>
Vector2<coord> Matrix3<coord>::multiplyPoint(const Vector2<coord> s) const {
  Vector2<coord> v;
  v.x = at(0, 0) * s.x + at(0, 1) * s.y + at(0, 2) * static_cast<coord>(1);
  v.y = at(1, 0) * s.x + at(1, 1) * s.y + at(1, 2) * static_cast<coord>(1);
  return v;
}

template <typename coord>
auto operator*(const Matrix3<coord> &m1, const Matrix3<coord> &m2) -> Matrix3<coord> {
  Matrix3<coord> result;
  for (int row = 0; row < 3; ++row)
    for (int col = 0; col < 3; ++col) {
      coord sum = static_cast<coord>(0);
      for (int k = 0; k < 3; ++k) sum += m1(row, k) * m2(k, col);
      result(row, col) = sum;
    }
  return result;
}

#ifdef DONT
template <typename coord>
auto Matrix3<coord>::multiplyVector(const Vector3<coord> v, *vout) const -> Vector3<coord> {
  vout->x = at(0, 0) * v.x + at(0, 1) * v.y + at(0, 2) * v.z;
  vout->y = at(1, 0) * v.x + at(1, 1) * v.y + at(1, 2) * v.z;
  vout->z = at(2, 0) * v.x + at(2, 1) * v.y + at(2, 2) * v.z;
}
#endif

template <typename coord>
auto Matrix3<coord>::determinant() const -> coord {
  return at(0, 0) * at(1, 1) * at(2, 2) + at(0, 1) * at(1, 2) * at(2, 0) + at(0, 2) * at(1, 0) * at(2, 1) -
         at(0, 2) * at(1, 1) * at(2, 0) - at(0, 1) * at(1, 0) * at(2, 2) - at(0, 0) * at(1, 2) * at(2, 1);
}

template <typename coord>
auto Matrix3<coord>::invert() const -> Matrix3<coord> {
  coord det = 1 / determinant();
  return Matrix3<coord>{
      det * (at(1, 1) * at(2, 2) - at(1, 2) * at(2, 1)),  -det * (at(0, 1) * at(2, 2) - at(0, 2) * at(2, 1)),
      det * (at(0, 1) * at(1, 2) - at(0, 2) * at(1, 1)),

      -det * (at(1, 0) * at(2, 2) - at(1, 2) * at(2, 0)), det * (at(0, 0) * at(2, 2) - at(0, 2) * at(2, 0)),
      -det * (at(0, 0) * at(1, 2) - at(0, 2) * at(1, 0)),

      det * (at(1, 0) * at(2, 1) - at(1, 1) * at(2, 0)),  -det * (at(0, 0) * at(2, 1) - at(0, 1) * at(2, 0)),
      det * (at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0))};
}

template <typename coord>
auto Matrix3<coord>::transpose() const -> Matrix3<coord> {
  return Matrix3<coord>{at(0, 0), at(1, 0), at(2, 0), at(0, 1), at(1, 1), at(2, 1), at(0, 2), at(1, 2), at(2, 2)};
}

//  Matrix Creation

template <typename coord>
auto Matrix3<coord>::makeUnit() -> Matrix3<coord> {
  return Matrix3<coord>{1, 0, 0, 0, 1, 0, 0, 0, 1};
}

template <typename coord>
auto Matrix3<coord>::makeTranslation(const Vector2<coord> &offset) -> Matrix3<coord> {
  return Matrix3<coord>{1, 0, offset.x, 0, 1, offset.y, 0, 0, 1};
}

template <typename coord>
auto Matrix3<coord>::makeScale(coord scaleX, coord scaleY) -> Matrix3<coord> {
  return Matrix3<coord>{scaleX,
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        scaleY,
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        static_cast<coord>(1)};
}

template <typename coord>
auto Matrix3<coord>::makeRotation(coord angle) -> Matrix3<coord> {
  coord sine = std::sin(angle);
  coord cosine = std::cos(angle);
  return Matrix3<coord>{cosine, -sine, 0, sine, cosine, 0, 0, 0, 1};
}

template <typename coord>
auto Matrix3<coord>::makeShear(coord factorX, coord factorY) -> Matrix3<coord> {
  return Matrix3<coord>{1, factorX, 0, factorY, 1, 0, 0, 0, 1};
}

///////////////////////////////////////////////////////////
// Matrix4<coord> implementation

template <typename coord>
Matrix4<coord>::Matrix4() : Matrix4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) {}

template <typename coord>
Matrix4<coord>::Matrix4(coord m11, coord m12, coord m13, coord m14, coord m21, coord m22, coord m23, coord m24,
                        coord m31, coord m32, coord m33, coord m34, coord m41, coord m42, coord m43, coord m44) {
  at(0, 0) = m11;
  at(0, 1) = m12;
  at(0, 2) = m13;
  at(0, 3) = m14;
  at(1, 0) = m21;
  at(1, 1) = m22;
  at(1, 2) = m23;
  at(1, 3) = m24;
  at(2, 0) = m31;
  at(2, 1) = m32;
  at(2, 2) = m33;
  at(2, 3) = m34;
  at(3, 0) = m41;
  at(3, 1) = m42;
  at(3, 2) = m43;
  at(3, 3) = m44;
}

template <typename coord>
template <typename othercoord>
Matrix4<coord>::Matrix4(const Matrix4<othercoord> &other) {
  at(0, 0) = static_cast<coord>(other(0, 0));
  at(0, 1) = static_cast<coord>(other(0, 1));
  at(0, 2) = static_cast<coord>(other(0, 2));
  at(0, 3) = static_cast<coord>(other(0, 3));
  at(1, 0) = static_cast<coord>(other(1, 0));
  at(1, 1) = static_cast<coord>(other(1, 1));
  at(1, 2) = static_cast<coord>(other(1, 2));
  at(1, 3) = static_cast<coord>(other(1, 3));
  at(2, 0) = static_cast<coord>(other(2, 0));
  at(2, 1) = static_cast<coord>(other(2, 1));
  at(2, 2) = static_cast<coord>(other(2, 2));
  at(2, 3) = static_cast<coord>(other(2, 3));
  at(3, 0) = static_cast<coord>(other(3, 0));
  at(3, 1) = static_cast<coord>(other(3, 1));
  at(3, 2) = static_cast<coord>(other(3, 2));
  at(3, 3) = static_cast<coord>(other(3, 3));
}

template <typename coord>
auto Matrix4<coord>::makeUnit() -> Matrix4<coord> {
  return Matrix4<coord>{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
}

template <typename coord>
auto Matrix4<coord>::makeTranslation(const Vector3<coord> &offset) -> Matrix4<coord> {
  return Matrix4<coord>{1, 0, 0, offset.x, 0, 1, 0, offset.y, 0, 0, 1, offset.z, 0, 0, 0, 1};
}

template <typename coord>
auto Matrix4<coord>::makeScale(const Vector3<coord> &scale) -> Matrix4<coord> {
  return Matrix4<coord>{scale.x,
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        scale.y,
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        scale.z,
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        static_cast<coord>(0),
                        static_cast<coord>(1)};
}

template <typename coord>
auto Matrix4<coord>::makeRotationX(coord angle) -> Matrix4<coord> {
  coord sine = std::sin(angle);
  coord cosine = std::cos(angle);
  return Matrix4<coord>{1, 0, 0, 0, 0, cosine, -sine, 0, 0, sine, cosine, 0, 0, 0, 0, 1};
}

template <typename coord>
auto Matrix4<coord>::makeRotationY(coord angle) -> Matrix4<coord> {
  coord sine = std::sin(angle);
  coord cosine = std::cos(angle);
  return Matrix4<coord>{cosine, 0, sine, 0, 0, 1, 0, 0, -sine, 0, cosine, 0, 0, 0, 0, 1};
}

template <typename coord>
auto Matrix4<coord>::makeRotationZ(coord angle) -> Matrix4<coord> {
  coord sine = std::sin(angle);
  coord cosine = std::cos(angle);
  return Matrix4<coord>{cosine, -sine, 0, 0, sine, cosine, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
}

#ifdef DONT
template <typename coord>
auto Matrix4<coord>::makeShearXY(coord shx, coord shy) -> Matrix4<coord> {
  return Matrix4<coord>{1, 0, shx, 0, 0, 1, shy, 0, 0, 0, 1, 0, 0, 0, 0, 1};
}
#endif

template <typename coord>
auto Matrix4<coord>::makeShearXZ(coord shx, coord shy) -> Matrix4<coord> {
  return Matrix4<coord>{1, 0, shx, 0, 0, 1, shy, 0, 0, 0, 1, 0, 0, 0, 0, 1};
}

template <typename coord>
auto Matrix4<coord>::makeShearXY(coord shx, coord shy) -> Matrix4<coord> {
  return Matrix4<coord>{1, 0, shx, 0, 0, 1, shy, 0, 0, 0, 1, 0, 0, 0, 0, 1};
}

template <typename coord>
auto Matrix4<coord>::makePerspective(coord fovy, coord aspect, coord near, coord far) -> Matrix4<coord> {
  auto f = 1.0 / tan(fovy / 2.0);
  auto nf = 1.0 / (near - far);

  auto m22 = (far + near) * nf;
  auto m23 = 2.0 * far * near * nf;

  // TODO(ib@unfolded.ai): Doesn't support far not being set or far being Infinity
  return Matrix4<coord>{f / aspect, 0, 0, 0, 0, f, 0, 0, 0, 0, m22, m23, 0, 0, static_cast<coord>(-1), 0};
}

template <typename coord>
auto Matrix4<coord>::transpose() const -> Matrix4<coord> {
  return Matrix4<coord>{at(0, 0), at(1, 0), at(2, 0), at(3, 0), at(0, 1), at(1, 1), at(2, 1), at(3, 1),
                        at(0, 2), at(1, 2), at(2, 2), at(3, 2), at(0, 3), at(1, 3), at(2, 3), at(3, 3)};
}

template <typename coord>
auto Matrix4<coord>::isHomogeneous() const -> bool {
  return at(3, 0) == 0 && at(3, 1) == 0 && at(3, 2) == 0 && at(3, 3) == 1;
}

template <typename coord>
auto Matrix4<coord>::multiplyVector(const Vector3<coord> &s) const -> Vector3<coord> {
  return Vector3<coord>{at(0, 0) * s.x + at(0, 1) * s.y + at(0, 2) * s.z,
                        at(1, 0) * s.x + at(1, 1) * s.y + at(1, 2) * s.z,
                        at(2, 0) * s.x + at(2, 1) * s.y + at(2, 2) * s.z};
}

template <typename coord>
auto Matrix4<coord>::multiplyPoint(const Vector3<coord> &s) const -> Vector3<coord> {
  // TODO(ib) - ???
  // coord w = at(3, 0) * s.x + at(3, 1) * s.y + at(3, 2) * s.z + at(3, 3) *
  // static_cast<coord>(1); if (abs (w - 1.0) > FLT_EPSILON) throw
  // std::runtime_error ("error");

  return Vector3<coord>{at(0, 0) * s.x + at(0, 1) * s.y + at(0, 2) * s.z + at(0, 3) * static_cast<coord>(1),
                        at(1, 0) * s.x + at(1, 1) * s.y + at(1, 2) * s.z + at(1, 3) * static_cast<coord>(1),
                        at(2, 0) * s.x + at(2, 1) * s.y + at(2, 2) * s.z + at(2, 3) * static_cast<coord>(1)};
}

template <typename coord>
auto Matrix4<coord>::scale(const Vector3<coord> &s) const -> Matrix4<coord> {
  auto scaleMatrix = Matrix4<coord>::makeScale(s);
  return *this * scaleMatrix;

  /*
  return Matrix4<coord>{m[0][0] * s.x, m[0][1] * s.y, m[0][2] * s.z, m[0][3],
                        m[1][0] * s.x, m[1][1] * s.y, m[1][2] * s.z, m[1][3],
                        m[2][0] * s.x, m[2][1] * s.y, m[2][2] * s.z, m[2][3],
                        m[3][0] * s.x, m[3][1] * s.y, m[3][2] * s.z, m[3][3]};
  */
}

template <typename coord>
auto Matrix4<coord>::translate(const Vector3<coord> &t) const -> Matrix4<coord> {
  auto translationMatrix = Matrix4<coord>::makeTranslation(t);
  return *this * translationMatrix;

  /*
  auto m03 = m[0][0] * t.x + m[0][1] * t.y + m[0][2] * t.z + m[0][3];
  auto m13 = m[1][0] * t.x + m[1][1] * t.y + m[1][2] * t.z + m[1][3];
  auto m23 = m[2][0] * t.x + m[2][1] * t.y + m[2][2] * t.z + m[2][3];
  auto m33 = m[3][0] * t.x + m[3][1] * t.y + m[3][2] * t.z + m[3][3];

  return Matrix4<coord>{m[0][0], m[0][1], m[0][2], m03,
                        m[1][0], m[1][1], m[1][2], m13,
                        m[2][0], m[2][1], m[2][2], m23,
                        m[3][0], m[3][1], m[3][2], m33};
  */
}

template <typename coord>
auto Matrix4<coord>::rotateX(const coord rad) -> Matrix4<coord> {
  auto rotationVector = Matrix4<coord>::makeRotationX(rad);
  return *this * rotationVector;

  /*
  auto s = sin(rad);
  auto c = cos(rad);

  auto m01 = m[0][1] * c + m[0][2] * s;
  auto m11 = m[1][1] * c + m[1][2] * s;
  auto m21 = m[2][1] * c + m[2][2] * s;
  auto m31 = m[3][1] * c + m[3][2] * s;

  auto m02 = m[0][2] * c - m[0][1] * s;
  auto m12 = m[1][2] * c - m[1][1] * s;
  auto m22 = m[2][2] * c - m[2][1] * s;
  auto m32 = m[3][2] * c - m[3][1] * s;

  return Matrix4<coord>{m[0][0], m01, m02, m[0][3],
                        m[1][0], m11, m12, m[1][3],
                        m[2][0], m21, m22, m[2][3],
                        m[3][0], m31, m32, m[3][3]};
  */
}

template <typename coord>
auto Matrix4<coord>::rotateY(const coord rad) -> Matrix4<coord> {
  auto rotationVector = Matrix4<coord>::makeRotationY(rad);
  return *this * rotationVector;

  /*
  auto s = sin(rad);
  auto c = cos(rad);

  auto m00 = m[0][0] * c - m[0][2] * s;
  auto m10 = m[1][0] * c - m[1][2] * s;
  auto m20 = m[2][0] * c - m[2][2] * s;
  auto m30 = m[3][0] * c - m[3][2] * s;

  auto m02 = m[0][0] * s + m[0][2] * c;
  auto m12 = m[1][0] * s + m[1][2] * c;
  auto m22 = m[2][0] * s + m[2][2] * c;
  auto m32 = m[3][0] * s + m[3][2] * c;

  return Matrix4<coord>{m00, m[0][1], m02, m[0][3],
                        m10, m[1][1], m12, m[1][3],
                        m20, m[2][1], m22, m[2][3],
                        m30, m[3][1], m32, m[3][3]};
  */
}

template <typename coord>
auto Matrix4<coord>::rotateZ(const coord rad) -> Matrix4<coord> {
  auto rotationVector = Matrix4<coord>::makeRotationZ(rad);
  return *this * rotationVector;
}

template <typename coord>
auto Matrix4<coord>::determinant() const -> coord {
  return at(0, 0) * at(1, 1) * at(2, 2) * at(3, 3) - at(0, 0) * at(1, 1) * at(3, 2) * at(2, 3) -
         at(0, 0) * at(2, 1) * at(1, 2) * at(3, 3) + at(0, 0) * at(2, 1) * at(3, 2) * at(1, 3) +
         at(0, 0) * at(3, 1) * at(1, 2) * at(2, 3) - at(0, 0) * at(3, 1) * at(2, 2) * at(1, 3) -
         at(1, 0) * at(0, 1) * at(2, 2) * at(3, 3) + at(1, 0) * at(0, 1) * at(3, 2) * at(2, 3) +
         at(1, 0) * at(2, 1) * at(0, 2) * at(3, 3) - at(1, 0) * at(2, 1) * at(3, 2) * at(0, 3) -
         at(1, 0) * at(3, 1) * at(0, 2) * at(2, 3) + at(1, 0) * at(3, 1) * at(2, 2) * at(0, 3) +
         at(2, 0) * at(0, 1) * at(1, 2) * at(3, 3) - at(2, 0) * at(0, 1) * at(3, 2) * at(1, 3) -
         at(2, 0) * at(1, 1) * at(0, 2) * at(3, 3) + at(2, 0) * at(1, 1) * at(3, 2) * at(0, 3) +
         at(2, 0) * at(3, 1) * at(0, 2) * at(1, 3) - at(2, 0) * at(3, 1) * at(1, 2) * at(0, 3) -
         at(3, 0) * at(0, 1) * at(1, 2) * at(2, 3) + at(3, 0) * at(0, 1) * at(2, 2) * at(1, 3) +
         at(3, 0) * at(1, 1) * at(0, 2) * at(2, 3) - at(3, 0) * at(1, 1) * at(2, 2) * at(0, 3) -
         at(3, 0) * at(2, 1) * at(0, 2) * at(1, 3) + at(3, 0) * at(2, 1) * at(1, 2) * at(0, 3);
}

#ifdef DONT
template <typename coord>
auto Matrix4<coord>::RotationMatrix(coord a_x, coord a_y, coord a_z) -> Matrix4<coord> {
  return Matrix{cos(a_z) * cos(a_y),
                -sin(a_z) * cos(a_y),
                sin(a_y),
                0,
                sin(a_z) * cos(a_x) + cos(a_z) * sin(a_y) * sin(a_x),
                cos(a_z) * cos(a_x) - sin(a_z) * sin(a_y) * sin(a_x),
                -cos(a_y) * sin(a_x),
                0,
                sin(a_z) * sin(a_x) - cos(a_z) * sin(a_y) * cos(a_x),
                cos(a_z) * sin(a_x) + sin(a_z) * sin(a_y) * cos(a_x),
                cos(a_y) * cos(a_x),
                0,
                0,
                0,
                0,
                1};
}
#endif

#ifdef DONT
template <typename coord>
auto Matrix4<coord>::multiplyPoint(const Vector3<coord> s) const -> Vector3<coord> {
  return Vector3<coord> {
    v.x = at(0, 0) * s.x + at(1, 0) * s.y + at(2, 0) * s.z + at(3, 0) * static_cast<coord>(1);
    v.y = at(0, 1) * s.x + at(1, 1) * s.y + at(2, 1) * s.z + at(3, 1) * static_cast<coord>(1);
    v.z = at(0, 2) * s.x + at(1, 2) * s.y + at(2, 2) * s.z + at(3, 2) * static_cast<coord>(1)
  }
}
#endif

template <class T>
std::ostream &operator<<(std::ostream &, Matrix4<T> &);
template <class T>
std::istream &operator>>(std::istream &, Matrix4<T> &);

// Matrix Algebra
// friend typename Matrix operator- (const typename Matrix &, const typename
// Matrix &);
template <class T>
auto operator*(const Matrix4<T> &, const Matrix4<T> &) -> Matrix4<T>;
template <class T>
auto operator+(const Matrix4<T> &, const Matrix4<T> &) -> Matrix4<T>;

//  Matrix algebra

template <typename coord>
auto operator*(const Matrix4<coord> &m1, const Matrix4<coord> &m2) -> Matrix4<coord> {
  Matrix4<coord> result;
  for (int row = 0; row < 4; ++row)
    for (int col = 0; col < 4; ++col) {
      coord sum = static_cast<coord>(0);
      for (int k = 0; k < 4; ++k) sum += m1(row, k) * m2(k, col);
      result(row, col) = sum;
    }

  return result;
}

}  // namespace mathgl

#endif  // MATHGL_CORE_CORE_H
