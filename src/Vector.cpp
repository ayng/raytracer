#include <cmath>
#include <cstdio>
#include "Vector.h"

/* Vector3 */

Vector3::Vector3 () {
  x = 0;
  y = 0;
  z = 0;
}

Vector3::Vector3 (double _x, double _y, double _z) {
  x = _x;
  y = _y;
  z = _z;
}

Vector3::Vector3 (const Vector3& orig) {
  x = orig.x;
  y = orig.y;
  z = orig.z;
}

Vector3 Vector3::operator + (const Vector3& rhs) const {
  Vector3 res(*this);
  res.x += rhs.x;
  res.y += rhs.y;
  res.z += rhs.z;
  return res;
}

Vector3 Vector3::operator - (const Vector3& rhs) const {
  Vector3 res(*this);
  res.x -= rhs.x;
  res.y -= rhs.y;
  res.z -= rhs.z;
  return res;
}

Vector3 Vector3::operator * (const double rhs) const {
  Vector3 res(*this);
  res.x *= rhs;
  res.y *= rhs;
  res.z *= rhs;
  return res;
}

bool Vector3::operator == (const Vector3& rhs) const {
  return x == rhs.x && y == rhs.y && z == rhs.z;
}

bool Vector3::isDefined () const {
  return !(isnan(x) || isnan(y) || isnan(z));
}

double Vector3::dot(const Vector3& rhs) const {
  return x * rhs.x + y * rhs.y + z * rhs.z;
}
Vector3 Vector3::cross(const Vector3& rhs) const {
  return Vector3(y * rhs.z - z * rhs.y,
                 z * rhs.x - x * rhs.z,
                 x * rhs.y - y * rhs.x);
}

double Vector3::magnitude() const {
  return sqrt(x*x + y*y + z*z);
}

Vector3 Vector3::normalized() const {
  Vector3 res(*this);
  double m = magnitude();
  res.x /= m;
  res.y /= m;
  res.z /= m;
  return res;
}

void Vector3::dump() const {
  printf("<%.3f, %.3f, %.3f>\n", x, y, z);
}

Vector3 operator * (const double lhs, const Vector3& rhs) {
  return rhs * lhs;
}

/* Vector2 */

Vector2::Vector2 () {
  x = 0;
  y = 0;
}

Vector2::Vector2 (double _x, double _y) {
  x = _x;
  y = _y;
}

Vector2::Vector2 (const Vector2& orig) {
  x = orig.x;
  y = orig.y;
}

Vector2 Vector2::operator + (const Vector2& rhs) const {
  Vector2 res(*this);
  res.x += rhs.x;
  res.y += rhs.y;
  return res;
}

Vector2 Vector2::operator - (const Vector2& rhs) const {
  Vector2 res(*this);
  res.x -= rhs.x;
  res.y -= rhs.y;
  return res;
}

double Vector2::magnitude () const {
  return sqrt(x*x + y*y);
}
