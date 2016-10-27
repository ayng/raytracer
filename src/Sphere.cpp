#include "Vector.h"
#include "Sphere.h"

Sphere::Sphere (double cx, double cy, double cz, double r, Matrix4 i, Matrix4 o)
  : center (cx, cy, cz), in (i), out (o) {
  radius = r;
}
