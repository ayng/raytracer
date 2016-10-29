/** Copyright 2016 Alex Yang */
#include "Geometry.h"
#include "Vector.h"

Geometry::Geometry() {}
Geometry::Geometry(Material mat, Matrix4 w2o, Matrix4 o2w)
  : worldToObject(w2o), objectToWorld(o2w) {
  material = mat;
}

Triangle::Triangle() {}
Triangle::Triangle(Vector3 _p1, Vector3 _p2, Vector3 _p3,
  Material mat, Matrix4 w2o, Matrix4 o2w)
  : p1(_p1), p2(_p2), p3(_p3), Geometry(mat, w2o, o2w) {}
Ray Triangle::intersect(const Ray& ray) {
  
}
