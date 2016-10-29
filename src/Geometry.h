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
  Vector3 a, b, c;
  Vector3 normal;

  Triangle();
  Triangle(Vector3 aa, Vector3 bb, Vector3 cc,
    Material mat, Matrix4 w2o, Matrix4 o2w);
  Ray intersect(const Ray& ray);
};

class Sphere : public Geometry {
 public:
  Vector3 center;
  double radius;

  Sphere();
  Sphere(Vector3 c, double r, Material mat, Matrix4 w2o, Matrix4 o2w);
  Ray intersect(const Ray& ray);
};

#endif  // SRC_GEOMETRY_H_
