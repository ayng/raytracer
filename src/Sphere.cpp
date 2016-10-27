#include "Vector.h"
#include "Sphere.h"

#include "Material.h"

Sphere::Sphere (double cx, double cy, double cz, double r, Material mat, Matrix4 i, Matrix4 o)
  : center (cx, cy, cz), in (i), out (o) {
  radius = r;
  material = mat;
}
