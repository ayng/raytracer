#include "Color.h"
#include <algorithm>
#include <cstdio>

double bound (const double n, const double lower, const double upper) {
  return std::min(upper, std::max(lower, n));
}

Color::Color () {
  r = 0;
  g = 0;
  b = 0;
}

Color::Color (const double _r, const double _g, const double _b) {
  r = _r;
  g = _g;
  b = _b;
}

Color::Color (const Color& orig) {
  r = orig.r;
  g = orig.g;
  b = orig.b;
}

Color Color::operator + (const Color& rhs) const {
  Color res(*this);
  res.r = bound(rhs.r + res.r, 0.0, 1.0);
  res.g = bound(rhs.g + res.g, 0.0, 1.0);
  res.b = bound(rhs.b + res.b, 0.0, 1.0);
  return res;
}

Color Color::operator * (const double rhs) const {
  Color res(*this);
  res.r = bound(res.r * rhs, 0.0, 1.0);
  res.g = bound(res.g * rhs, 0.0, 1.0);
  res.b = bound(res.b * rhs, 0.0, 1.0);
  return res;
}

Color Color::operator * (const Color& rhs) const {
  Color res(*this);
  res.r = bound(res.r * rhs.r, 0.0, 1.0);
  res.g = bound(res.g * rhs.g, 0.0, 1.0);
  res.b = bound(res.b * rhs.b, 0.0, 1.0);
  return res;
}

Color operator * (const double lhs, const Color& rhs) {
  return rhs * lhs;
}

void Color::dump () const {
  printf("r:%.2f  g:%.2f  b:%.2f\n", r, g, b);
}
