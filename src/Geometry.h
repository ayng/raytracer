/** Copyright 2016 Alex Yang */
#ifndef SRC_GEOMETRY_H_
#define SRC_GEOMETRY_H_

#include "Material.h"
#include "Matrix.h"
#include "Vector.h"

class Geometry {
 public:
  Material material;
  Matrix4 worldToObject, objectToWorld;

  Geometry();
  Geometry(Material mat, Matrix4 w2o, Matrix4 o2w);
  virtual Ray intersect(const Ray& ray) = 0;
};

class Triangle : public Geometry {
 public:
  Vector3 p1, p2, p3;

  Triangle();
  Triangle(Vector3 p1, Vector3 p2, Vector3 p3,
    Material mat, Matrix4 w2o, Matrix4 o2w);
  Ray intersect(const Ray& ray);
};

#endif  // SRC_GEOMETRY_H_
