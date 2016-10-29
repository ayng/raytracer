#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "Vector.h"
#include "Sphere.h"

struct Intersection {
  Vector3 point;
  Vector3 normal;
  Sphere sphere;
};

#endif
