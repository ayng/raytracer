#include <algorithm>
#include <cstdio>
#include "Color.h"

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
  res.r = rhs.r + res.r;
  res.g = rhs.g + res.g;
  res.b = rhs.b + res.b;
  return res;
}

Color Color::operator * (const double rhs) const {
  Color res(*this);
  res.r = res.r * rhs;
  res.g = res.g * rhs;
  res.b = res.b * rhs;
  return res;
}

Color Color::operator * (const Color& rhs) const {
  Color res(*this);
  res.r = res.r * rhs.r;
  res.g = res.g * rhs.g;
  res.b = res.b * rhs.b;
  return res;
}

Color operator * (const double lhs, const Color& rhs) {
  return rhs * lhs;
}

Color Color::clipped() const {
  return Color(
    bound(r, 0.0, 1.0),
    bound(g, 0.0, 1.0),
    bound(b, 0.0, 1.0)
  );
}

void Color::dump () const {
  printf("r:%.2f  g:%.2f  b:%.2f\n", r, g, b);
}
