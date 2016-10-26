#include "Vector.h"
#include "Sphere.h"

Sphere::Sphere (double cx, double cy, double cz, double r, Matrix4 t)
  : center (cx, cy, cz), transform (t) {
  radius = r;
}
