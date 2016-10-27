#ifndef SPHERE_H
#define SPHERE_H

#include "Vector.h"
#include "Matrix.h"

class Sphere {
  public:
    Matrix4 in, out;
    Vector3 center;
    double radius;
    Sphere ();
    Sphere (double cx, double cy, double cz, double r, Matrix4 i, Matrix4 o);
};

#endif
