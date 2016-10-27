#ifndef SPHERE_H
#define SPHERE_H

#include "Vector.h"
#include "Matrix.h"
#include "Material.h"

class Sphere {
  public:
    Material material;
    Matrix4 in, out;
    Vector3 center;
    double radius;
    Sphere ();
    Sphere (double cx, double cy, double cz, double r, Material mat, Matrix4 i, Matrix4 o);
};

#endif
