#ifndef SPHERE_H
#define SPHERE_H

#include "Vector.h"

class Sphere {
  public:
    Vector3 center;
    double radius;
    Sphere ();
    Sphere (double cx, double cy, double cz, double r);
};

#endif
