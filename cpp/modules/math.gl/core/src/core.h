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

  auto operator-() const -> Vector2<coord> { return Vector2<coord>(-x, -y); }
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

  coord Length() const;
  coord Length2() const;
  coord Angle() const;

  void Normalize();

  auto lerp(const Vector2<coord> &, coord t) -> Vector2<coord>;

  coord x, y;
};

template <typename coord>
auto ElementProduct(const Vector2<coord> &, const Vector2<coord> &) -> Vector2<coord>;

template <typename coord>
auto operator+(const Vector2<coord> &v1, const Vector2<coord> &v2) -> Vector2<coord> {
  return Vector2<coord>(v1.x + v2.x, v1.y + v2.y);
}

template <typename coord>
auto operator-(const Vector2<coord> &v1, const Vector2<coord> &v2) -> Vector2<coord> {
  return Vector2<coord>(v1.x - v2.x, v1.y - v2.y);
}

template <typename coord>
auto operator==(const Vector2<coord> &v1, const Vector2<coord> &v2) -> bool {
  return v1.x == v2.x && v1.y == v2.y;
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

  auto operator-() const -> Vector3<coord> { return Vector3<coord>(-x, -y, -z); }
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

  auto Length() const -> coord;
  auto Length2() const -> coord;
  auto toVector2() const -> Vector2<coord>;

  void Normalize();

  coord x, y, z;
};

template <typename coord>
auto VectorProduct(const Vector3<coord> &, const Vector3<coord> &) -> Vector3<coord>;

template <typename coord>
auto ElementProduct(const Vector3<coord> &, const Vector3<coord> &) -> Vector3<coord>;

template <typename coord>
auto operator+(const Vector3<coord> &v1, const Vector3<coord> &v2) -> Vector3<coord> {
  return Vector3<coord>(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

template <typename coord>
auto operator-(const Vector3<coord> &v1, const Vector3<coord> &v2) -> Vector3<coord> {
  return Vector3<coord>(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

template <typename coord>
auto operator==(const Vector3<coord> &v1, const Vector3<coord> &v2) -> bool {
  return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
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

  auto operator-() const -> Vector4<coord> { return Vector4<coord>(-x, -y, -z, -w); }

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
    return Vector4<coord>(x + v.x, y + v.y, z + v.z, w + v.w);
  }

  auto operator-(const Vector4<coord> &v) const -> Vector4<coord> {
    return Vector4<coord>(x - v.x, y - v.y, z - v.z, w - v.w);
  }

  auto operator==(const Vector4<coord> &v) const -> bool { return x == v.x && y == v.y && z == v.z && w == v.w; }

  coord Length() const;
  coord Length2() const;
  auto toVector3() const -> Vector3<coord>;

  void Normalize();

  coord x, y, z, w;
};

template <typename coord>
Vector4<coord> VectorProduct(const Vector4<coord> &, const Vector4<coord> &);

template <typename coord>
Vector4<coord> ElementProduct(const Vector4<coord> &, const Vector4<coord> &);

template <typename coord>
auto Vector4<coord>::toVector3() const -> Vector3<coord> {
  return Vector3<coord>(x, y, z);
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

  static auto MakeUnit() -> Matrix2<coord>;
  static auto MakeScale(coord sx, coord sy) -> Matrix2;
  static auto MakeRotation(coord angle) -> Matrix2;  // counterclockwise

  auto Invert() const -> Matrix2<coord>;

  auto operator()(unsigned row, unsigned col) -> coord & { return m[row][col]; }
  auto operator()(unsigned row, unsigned col) const -> const coord { return m[row][col]; }

  coord m[2][2];
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

  auto operator()(int row, int col) -> coord & { return m[row][col]; }
  auto operator()(int row, int col) const -> const coord { return m[row][col]; }

  // Matrix creation
  static auto MakeUnit() -> Matrix3<coord>;
  static auto MakeTranslation(const Vector2<coord> &offset) -> Matrix3<coord>;
  static auto MakeScale(coord scaleX, coord scaleY) -> Matrix3<coord>;
  static auto MakeRotation(coord angle) -> Matrix3<coord>;
  static auto MakeShear(coord factorX, coord factorY) -> Matrix3<coord>;

  // Matrix Algebra
  auto Determinant() const -> coord;
  auto Invert() const -> Matrix3<coord>;
  auto Transpose() const -> Matrix3<coord>;

  // Vector multiplication
  auto Multiply(const Vector3<coord>) const -> Vector3<coord>;

  // Homogenous coordinates
  auto IsHomogenous() const -> bool;
  auto MultiplyPoint(const Vector2<coord>) const -> Vector2<coord>;
  auto MultiplyVector(const Vector2<coord>) const -> Vector2<coord>;

  coord m[3][3];
};

template <class T>
auto operator+(const Matrix3<T> &, const Matrix3<T> &) -> Matrix3<T>;
template <class T>
auto operator-(const Matrix3<T> &, const Matrix3<T> &) -> Matrix3<T>;
template <class T>
auto operator*(const Matrix3<T> &, const Matrix3<T> &) -> Matrix3<T>;

template <class T>
auto operator<<(std::ostream &, Matrix3<T> &) -> std::ostream &;
template <class T>
auto operator>>(std::istream &, Matrix3<T> &) -> std::istream &;

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

  auto operator=(const Matrix4<coord> &) -> Matrix4<coord> & = default;
  auto operator=(Matrix4<coord> &&) -> Matrix4<coord> & = default;

  // Matrix Creation
  static auto MakeUnit() -> Matrix4<coord>;
  static auto MakeTranslation(const Vector3<coord> &offset) -> Matrix4<coord>;
  static auto MakeScale(const Vector3<coord> &scale) -> Matrix4<coord>;
  static auto MakeRotationX(coord angle) -> Matrix4<coord>;
  static auto MakeRotationY(coord angle) -> Matrix4<coord>;
  static auto MakeRotationZ(coord angle) -> Matrix4<coord>;
  static auto MakeShearXY(coord factorX, coord factorY) -> Matrix4<coord>;
  static auto MakeShearXZ(coord factorX, coord factorZ) -> Matrix4<coord>;
  static auto MakeShearYX(coord factorY, coord factorZ) -> Matrix4<coord>;
  static auto MakeProjection(coord distance) -> Matrix4<coord>;
  static auto makePerspective(coord fovy, coord aspect, coord near, coord far) -> Matrix4<coord>;

  auto operator()(int row, int col) -> coord & { return m[row][col]; }
  auto operator()(int row, int col) const -> const coord { return m[row][col]; }

  auto Determinant() const -> coord;
  auto Invert() const -> Matrix4<coord>;
  auto Transpose() const -> Matrix4<coord>;

  bool IsHomogenous() const;
  auto MultiplyVector(const Vector3<coord> &) const -> Vector3<coord>;
  auto MultiplyPoint(const Vector3<coord> &) const -> Vector3<coord>;

  auto scale(const Vector3<coord> &) const -> Matrix4<coord>;
  auto transform(const Vector4<coord> &) const -> Vector4<coord>;

  coord m[4][4];
};

template <typename coord>
auto operator==(const Matrix4<coord> &m1, const Matrix4<coord> &m2) -> bool {
  return m1.m[0][0] == m2.m[0][0] && m1.m[0][1] == m2.m[0][1] && m1.m[0][2] == m2.m[0][2] && m1.m[0][3] == m2.m[0][3] &&
         m1.m[1][0] == m2.m[1][0] && m1.m[1][1] == m2.m[1][1] && m1.m[1][2] == m2.m[1][2] && m1.m[1][3] == m2.m[1][3] &&
         m1.m[2][0] == m2.m[2][0] && m1.m[2][1] == m2.m[2][1] && m1.m[2][2] == m2.m[2][2] && m1.m[2][3] == m2.m[2][3] &&
         m1.m[3][0] == m2.m[3][0] && m1.m[3][1] == m2.m[3][1] && m1.m[3][2] == m2.m[3][2] && m1.m[3][3] == m2.m[3][3];
}

template <typename coord>
auto operator!=(const Matrix4<coord> &v1, const Matrix4<coord> &v2) -> bool {
  return !(v1 == v2);
}

///////////////////////////////////////////////////////////
// Vector2 implementation

template <typename coord>
auto Vector2<coord>::Length() const -> coord {
  return static_cast<coord>(std::sqrt(x * x + y * y));
}

template <typename coord>
auto Vector2<coord>::Length2() const -> coord {
  return static_cast<coord>(x * x + y * y);
}

template <typename coord>
auto Vector2<coord>::Angle() const -> coord {
  if (x == static_cast<coord>(0) && y == static_cast<coord>(0)) return static_cast<coord>(0);
  return static_cast<coord>(atan2(y, x));
}

template <typename coord>
void Vector2<coord>::Normalize() {
  coord length = Length();
  if (length <= static_cast<coord>(0)) throw std::logic_error("normalize called on zero length vector");
  x /= length;
  y /= length;
}

template <typename coord>
auto Vector2<coord>::lerp(const Vector2<coord> &v, coord t) -> Vector2<coord> {
  return Vector2<coord>(x + t * (v.x - x), y + t * (v.y - y));
}

template <typename coord>
auto ElementProduct(const Vector2<coord> &u, const Vector2<coord> &v) -> Vector2<coord> {
  return Vector2<coord>(u.x * v.x, u.y * v.y);
}

template <typename coord>
coord operator*(const Vector2<coord> &u, const Vector2<coord> &v) {
  return u.x * v.x + u.y * v.y;
}

template <typename coord>
auto operator*(const Vector2<coord> &v, coord c) -> Vector2<coord> {
  return Vector2<coord>(v.x * c, v.y * c);
}

template <typename coord>
auto operator*(coord c, const Vector2<coord> &v) -> Vector2<coord> {
  return Vector2<coord>(v.x * c, v.y * c);
}

template <typename coord>
auto operator/(const Vector2<coord> &v, coord c) -> Vector2<coord> {
  return Vector2<coord>(v.x / c, v.y / c);
}

template <typename coord>
auto operator/(coord c, const Vector2<coord> &v) -> Vector2<coord> {
  return Vector2<coord>(v.x / c, v.y / c);
}

template <typename coord>
auto operator<<(std::ostream &os, const Vector2<coord> &v) -> std::ostream & {
  os << "(" << v.x << "," << v.y << ")";
  return os;
}

///////////////////////////////////////////////////////////
// Vector3 implementation

template <typename coord>
auto Vector3<coord>::Length() const -> coord {
  return static_cast<coord>(sqrt(x * x + y * y + z * z));
}

template <typename coord>
auto Vector3<coord>::Length2() const -> coord {
  return static_cast<coord>(x * x + y * y + z * z);
}

template <typename coord>
auto Vector3<coord>::toVector2() const -> Vector2<coord> {
  return Vector2<coord>(this->x, this->y);
}

template <typename coord>
void Vector3<coord>::Normalize() {
  coord length = Length();
  if (length <= static_cast<coord>(0)) throw std::runtime_error("normalize called on zero length vector");
  x /= length;
  y /= length;
  z /= length;
}

template <typename coord>
auto VectorProduct(const Vector3<coord> &u, const Vector3<coord> &v) -> Vector3<coord> {
  return Vector3<coord>(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

template <typename coord>
auto ElementProduct(const Vector3<coord> &u, const Vector3<coord> &v) -> Vector3<coord> {
  return Vector3<coord>(u.x * v.x, u.y * v.y, u.z * v.z);
}

template <typename coord>
auto operator*(const Vector3<coord> &u, const Vector3<coord> &v) -> coord {
  return u.x * v.x + u.y * v.y + u.z * v.z;
}

template <typename coord>
auto operator*(const Vector3<coord> &v, coord c) -> Vector3<coord> {
  return Vector3<coord>(v.x * c, v.y * c, v.z * c);
}

template <typename coord>
auto operator*(coord c, const Vector3<coord> &v) -> Vector3<coord> {
  return Vector3<coord>(v.x * c, v.y * c, v.z * c);
}

template <typename coord>
auto operator/(const Vector3<coord> &v, coord c) -> Vector3<coord> {
  return Vector3<coord>(v.x / c, v.y / c, v.z / c);
}

template <typename coord>
auto operator/(coord c, const Vector3<coord> &v) -> Vector3<coord> {
  return Vector3<coord>(v.x / c, v.y / c, v.z / c);
}

template <typename coord>
auto operator<<(std::ostream &os, const Vector3<coord> &v) -> std::ostream & {
  os << "(" << v.x << "," << v.y << "," << v.z << ")";
  return os;
}

///////////////////////////////////////////////////////////
// Matrix2 implementation

template <typename coord>
Matrix2<coord>::Matrix2(coord a11, coord a12, coord a21, coord a22) {
  m[0][0] = a11;
  m[0][1] = a12;
  m[1][0] = a21;
  m[1][1] = a22;
}

template <typename coord>
Matrix2<coord>::Matrix2(const Matrix2<coord> &m2) {
  m[0][0] = m2.m[0][0];
  m[0][1] = m2.m[0][1];
  m[1][0] = m2.m[1][0];
  m[1][1] = m2.m[1][1];
}

template <typename coord>
auto Matrix2<coord>::MakeUnit() -> Matrix2<coord> {
  return Matrix2(1, 0, 0, 1);
}

template <typename coord>
auto Matrix2<coord>::MakeScale(coord sx, coord sy) -> Matrix2<coord> {
  return Matrix2(sx, static_cast<coord>(0), static_cast<coord>(0), sy);
}

template <typename coord>
auto Matrix2<coord>::MakeRotation(coord angle) -> Matrix2<coord> {
  coord sine = static_cast<coord>(sin(angle));
  coord cosine = static_cast<coord>(cos(angle));  // ERROR adapt function to precision?
  return Matrix2(cosine, -sine, sine, cosine);
}

template <typename coord>
auto Matrix2<coord>::Invert() const -> Matrix2<coord> {
  // Cramers rule
  coord det = m[0][0] * m[1][1] - m[0][1] * m[1][0];
  if (det == static_cast<coord>(0)) throw std::runtime_error("Attempt to invert singular matrix");
  det = 1 / det;
  return Matrix2<coord>(det * m[1][1], -det * m[0][1], -det * m[1][0], det * m[0][0]);
}

template <typename coord>
auto operator*(const Matrix2<coord> &m1, const Matrix2<coord> &m2) -> Matrix2<coord> {
  // Unrolled multiply
  return Matrix2<coord>(m1(0, 0) * m2(0, 0) + m1(0, 1) * m2(1, 0), m1(0, 0) * m2(1, 0) + m1(0, 1) * m2(1, 1),
                        m1(0, 1) * m2(0, 0) + m1(1, 1) * m2(1, 0), m1(0, 1) * m2(1, 0) + m1(1, 1) * m2(1, 1));
}

template <typename coord>
auto operator*(const Matrix2<coord> &m, const Vector2<coord> &v) -> Vector2<coord> {
  return Vector2<coord>(m(0, 0) * v.x + m(0, 1) * v.y, m(1, 0) * v.x + m(1, 1) * v.y);
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
  m[0][0] = a11;
  m[0][1] = a12;
  m[0][2] = a13;
  m[1][0] = a21;
  m[1][1] = a22;
  m[1][2] = a23;
  m[2][0] = a31;
  m[2][1] = a32;
  m[2][2] = a33;
}

template <typename coord>
Matrix3<coord>::Matrix3(const Matrix3<coord> &m2) {
  m[0][0] = m2.m[0][0];
  m[0][1] = m2.m[0][1];
  m[0][2] = m2.m[0][2];
  m[1][0] = m2.m[1][0];
  m[1][1] = m2.m[1][1];
  m[1][2] = m2.m[1][2];
  m[2][0] = m2.m[2][0];
  m[2][1] = m2.m[2][1];
  m[2][2] = m2.m[2][2];
}

template <typename coord>
bool Matrix3<coord>::IsHomogenous() const {
  return m[2][0] == 0 && m[2][1] == 0 && m[3][2] == 1;
}

template <typename coord>
Vector2<coord> Matrix3<coord>::MultiplyPoint(const Vector2<coord> s) const {
  Vector2<coord> v;
  v.x = m[0][0] * s.x + m[0][1] * s.y + m[0][2] * static_cast<coord>(1);
  v.y = m[1][0] * s.x + m[1][1] * s.y + m[1][2] * static_cast<coord>(1);
  return v;
}

template <typename coord>
auto operator*(const Matrix3<coord> &m1, const Matrix3<coord> &m2) -> Matrix3<coord> {
  Matrix3<coord> result;
  for (int row = 0; row < 3; ++row)
    for (int col = 0; col < 3; ++col) {
      coord sum = static_cast<coord>(0);
      for (int k = 0; k < 3; ++k) sum += m1.m[row][k] * m2.m[k][col];
      result(row, col) = sum;
    }
  return result;
}

#ifdef DONT
template <typename coord>
auto Matrix3<coord>::MultiplyVector(const Vector3<coord> v, *vout) const -> Vector3<coord> {
  vout->x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
  vout->y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
  vout->z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;
}
#endif

template <typename coord>
auto Matrix3<coord>::Determinant() const -> coord {
  return m[0][0] * m[1][1] * m[2][2] + m[0][1] * m[1][2] * m[2][0] + m[0][2] * m[1][0] * m[2][1] -
         m[0][2] * m[1][1] * m[2][0] - m[0][1] * m[1][0] * m[2][2] - m[0][0] * m[1][2] * m[2][1];
}

template <typename coord>
auto Matrix3<coord>::Invert() const -> Matrix3<coord> {
  coord det = 1 / Determinant();
  return Matrix3<coord>(det * (m[1][1] * m[2][2] - m[1][2] * m[2][1]), -det * (m[0][1] * m[2][2] - m[0][2] * m[2][1]),
                        det * (m[0][1] * m[1][2] - m[0][2] * m[1][1]),

                        -det * (m[1][0] * m[2][2] - m[1][2] * m[2][0]), det * (m[0][0] * m[2][2] - m[0][2] * m[2][0]),
                        -det * (m[0][0] * m[1][2] - m[0][2] * m[1][0]),

                        det * (m[1][0] * m[2][1] - m[1][1] * m[2][0]), -det * (m[0][0] * m[2][1] - m[0][1] * m[2][0]),
                        det * (m[0][0] * m[1][1] - m[0][1] * m[1][0]));
}

//  Matrix Creation

template <typename coord>
auto Matrix3<coord>::MakeUnit() -> Matrix3<coord> {
  return Matrix3<coord>(1, 0, 0, 0, 1, 0, 0, 0, 1);
}

template <typename coord>
auto Matrix3<coord>::MakeTranslation(const Vector2<coord> &offset) -> Matrix3<coord> {
  return Matrix3<coord>(1, 0, offset.x, 0, 1, offset.y, 0, 0, 1);
}

template <typename coord>
auto Matrix3<coord>::MakeScale(coord scaleX, coord scaleY) -> Matrix3<coord> {
  return Matrix3<coord>(scaleX, static_cast<coord>(0), static_cast<coord>(0), static_cast<coord>(0), scaleY,
                        static_cast<coord>(0), static_cast<coord>(0), static_cast<coord>(0), static_cast<coord>(1));
}

template <typename coord>
auto Matrix3<coord>::MakeRotation(coord angle) -> Matrix3<coord> {
  coord sine = std::sin(angle);
  coord cosine = std::cos(angle);
  return Matrix3<coord>(cosine, -sine, 0, sine, cosine, 0, 0, 0, 1);
}

///////////////////////////////////////////////////////////
// Matrix4<coord> implementation

template <typename coord>
Matrix4<coord>::Matrix4() : Matrix4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1) {}

template <typename coord>
Matrix4<coord>::Matrix4(coord m11, coord m12, coord m13, coord m14, coord m21, coord m22, coord m23, coord m24,
                        coord m31, coord m32, coord m33, coord m34, coord m41, coord m42, coord m43, coord m44) {
  m[0][0] = m11;
  m[0][1] = m12;
  m[0][2] = m13;
  m[0][3] = m14;
  m[1][0] = m21;
  m[1][1] = m22;
  m[1][2] = m23;
  m[1][3] = m24;
  m[2][0] = m31;
  m[2][1] = m32;
  m[2][2] = m33;
  m[2][3] = m34;
  m[3][0] = m41;
  m[3][1] = m42;
  m[3][2] = m43;
  m[3][3] = m44;
}

template <typename coord>
auto Matrix4<coord>::MakeUnit() -> Matrix4<coord> {
  return Matrix4<coord>(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

template <typename coord>
auto Matrix4<coord>::MakeTranslation(const Vector3<coord> &offset) -> Matrix4<coord> {
  return Matrix4<coord>(1, 0, 0, offset.x, 0, 1, 0, offset.y, 0, 0, 1, offset.z, 0, 0, 0, 1);
}

template <typename coord>
auto Matrix4<coord>::MakeScale(const Vector3<coord> &scale) -> Matrix4<coord> {
  return Matrix4<coord>(scale.x, static_cast<coord>(0), static_cast<coord>(0), static_cast<coord>(0),
                        static_cast<coord>(0), scale.y, static_cast<coord>(0), static_cast<coord>(0),
                        static_cast<coord>(0), static_cast<coord>(0), scale.z, static_cast<coord>(0),
                        static_cast<coord>(0), static_cast<coord>(0), static_cast<coord>(0), static_cast<coord>(1));
}

template <typename coord>
auto Matrix4<coord>::MakeRotationX(coord angle) -> Matrix4<coord> {
  coord sine = std::sin(angle);
  coord cosine = std::cos(angle);
  return Matrix4<coord>(1, 0, 0, 0, 0, cosine, -sine, 0, 0, sine, cosine, 0, 0, 0, 0, 1);
}

template <typename coord>
auto Matrix4<coord>::MakeRotationY(coord angle) -> Matrix4<coord> {
  coord sine = std::sin(angle);
  coord cosine = std::cos(angle);
  return Matrix4<coord>(cosine, 0, sine, 0, 0, 1, 0, 0, -sine, 0, cosine, 0, 0, 0, 0, 1);
}

template <typename coord>
auto Matrix4<coord>::MakeRotationZ(coord angle) -> Matrix4<coord> {
  coord sine = std::sin(angle);
  coord cosine = std::cos(angle);
  return Matrix4<coord>(cosine, -sine, 0, 0, sine, cosine, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

#ifdef DONT
template <typename coord>
auto Matrix4<coord>::MakeShearXY(coord shx, coord shy) -> Matrix4<coord> {
  return Matrix4<coord>(1, 0, shx, 0, 0, 1, shy, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}
#endif

template <typename coord>
auto Matrix4<coord>::MakeShearXZ(coord shx, coord shy) -> Matrix4<coord> {
  return Matrix4<coord>(1, 0, shx, 0, 0, 1, shy, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

template <typename coord>
auto Matrix4<coord>::MakeShearXY(coord shx, coord shy) -> Matrix4<coord> {
  return Matrix4<coord>(1, 0, shx, 0, 0, 1, shy, 0, 0, 0, 1, 0, 0, 0, 0, 1);
}

template <typename coord>
auto Matrix4<coord>::makePerspective(coord fovy, coord aspect, coord near, coord far) -> Matrix4<coord> {
  auto f = 1.0 / tan(fovy / 2.0);
  auto nf = 1.0 / (near - far);
  // TODO(ib@unfolded.ai): Doesn't support far not being set or far being Infinity
  return Matrix4<coord>(f / aspect, 0, 0, 0, 0, f, 0, 0, 0, 0, (far + near) * nf, -1, 0, 0, 2.0 * far * near * nf, 0);
}

template <typename coord>
auto Matrix4<coord>::IsHomogenous() const -> bool {
  return m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0 && m[3][3] == 1;
}

template <typename coord>
auto Matrix4<coord>::MultiplyVector(const Vector3<coord> &s) const -> Vector3<coord> {
  return Vector3<coord>(m[0][0] * s.x + m[0][1] * s.y + m[0][2] * s.z, m[1][0] * s.x + m[1][1] * s.y + m[1][2] * s.z,
                        m[2][0] * s.x + m[2][1] * s.y + m[2][2] * s.z);
}

template <typename coord>
auto Matrix4<coord>::MultiplyPoint(const Vector3<coord> &s) const -> Vector3<coord> {
  // TODO(ib) - ???
  // coord w = m[3][0] * s.x + m[3][1] * s.y + m[3][2] * s.z + m[3][3] *
  // static_cast<coord>(1); if (abs (w - 1.0) > FLT_EPSILON) throw
  // std::runtime_error ("error");

  return Vector3<coord>(m[0][0] * s.x + m[0][1] * s.y + m[0][2] * s.z + m[0][3] * static_cast<coord>(1),
                        m[1][0] * s.x + m[1][1] * s.y + m[1][2] * s.z + m[1][3] * static_cast<coord>(1),
                        m[2][0] * s.x + m[2][1] * s.y + m[2][2] * s.z + m[2][3] * static_cast<coord>(1));
}

template <typename coord>
auto Matrix4<coord>::scale(const Vector3<coord> &s) const -> Matrix4<coord> {
  return Matrix4<coord>(m[0][0] * s.x, m[0][1] * s.x, m[0][2] * s.x, m[0][3] * s.x, m[1][0] * s.y, m[1][1] * s.y,
                        m[1][2] * s.y, m[1][3] * s.y, m[2][0] * s.z, m[2][1] * s.z, m[2][2] * s.z, m[2][3] * s.z,
                        m[3][0], m[3][1], m[3][2], m[3][3]);
}

template <typename coord>
auto Matrix4<coord>::transform(const Vector4<coord> &xyzw) const -> Vector4<coord> {
  return Vector4<coord>(m[0][0] * xyzw.x + m[1][0] * xyzw.y + m[2][0] * xyzw.z + m[3][0] * xyzw.w,
                        m[0][1] * xyzw.x + m[1][1] * xyzw.y + m[2][1] * xyzw.z + m[3][1] * xyzw.w,
                        m[0][2] * xyzw.x + m[1][2] * xyzw.y + m[2][2] * xyzw.z + m[3][2] * xyzw.w,
                        m[0][3] * xyzw.x + m[1][3] * xyzw.y + m[2][3] * xyzw.z + m[3][3] * xyzw.w);
}

template <typename coord>
auto Matrix4<coord>::Determinant() const -> coord {
  return m[0][0] * m[1][1] * m[2][2] * m[3][3] - m[0][0] * m[1][1] * m[3][2] * m[2][3] -
         m[0][0] * m[2][1] * m[1][2] * m[3][3] + m[0][0] * m[2][1] * m[3][2] * m[1][3] +
         m[0][0] * m[3][1] * m[1][2] * m[2][3] - m[0][0] * m[3][1] * m[2][2] * m[1][3] -
         m[1][0] * m[0][1] * m[2][2] * m[3][3] + m[1][0] * m[0][1] * m[3][2] * m[2][3] +
         m[1][0] * m[2][1] * m[0][2] * m[3][3] - m[1][0] * m[2][1] * m[3][2] * m[0][3] -
         m[1][0] * m[3][1] * m[0][2] * m[2][3] + m[1][0] * m[3][1] * m[2][2] * m[0][3] +
         m[2][0] * m[0][1] * m[1][2] * m[3][3] - m[2][0] * m[0][1] * m[3][2] * m[1][3] -
         m[2][0] * m[1][1] * m[0][2] * m[3][3] + m[2][0] * m[1][1] * m[3][2] * m[0][3] +
         m[2][0] * m[3][1] * m[0][2] * m[1][3] - m[2][0] * m[3][1] * m[1][2] * m[0][3] -
         m[3][0] * m[0][1] * m[1][2] * m[2][3] + m[3][0] * m[0][1] * m[2][2] * m[1][3] +
         m[3][0] * m[1][1] * m[0][2] * m[2][3] - m[3][0] * m[1][1] * m[2][2] * m[0][3] -
         m[3][0] * m[2][1] * m[0][2] * m[1][3] + m[3][0] * m[2][1] * m[1][2] * m[0][3];
}

#ifdef DONT
template <typename coord>
auto Matrix4<coord>::RotationMatrix(coord a_x, coord a_y, coord a_z) -> Matrix4<coord> {
  return Matrix(cos(a_z) * cos(a_y), -sin(a_z) * cos(a_y), sin(a_y), 0,
                sin(a_z) * cos(a_x) + cos(a_z) * sin(a_y) * sin(a_x),
                cos(a_z) * cos(a_x) - sin(a_z) * sin(a_y) * sin(a_x), -cos(a_y) * sin(a_x), 0,
                sin(a_z) * sin(a_x) - cos(a_z) * sin(a_y) * cos(a_x),
                cos(a_z) * sin(a_x) + sin(a_z) * sin(a_y) * cos(a_x), cos(a_y) * cos(a_x), 0, 0, 0, 0, 1);
}
#endif

#ifdef DONT
template <typename coord>
auto Matrix4<coord>::MultiplyPoint(const Vector3<coord> s) const -> Vector3<coord> {
  return Vector3<coord>(v.x = m[0][0] * s.x + m[1][0] * s.y + m[2][0] * s.z + m[3][0] * static_cast<coord>(1);
                        v.y = m[0][1] * s.x + m[1][1] * s.y + m[2][1] * s.z + m[3][1] * static_cast<coord>(1);
                        v.z = m[0][2] * s.x + m[1][2] * s.y + m[2][2] * s.z + m[3][2] * static_cast<coord>(1))
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
      for (int k = 0; k < 4; ++k) sum += m1.m[row][k] * m2.m[k][col];
      result(row, col) = sum;
    }

  return result;
}

}  // namespace mathgl

#endif  // MATHGL_CORE_CORE_H
