/** Copyright 2016 Alex Yang */
#include <cstdio>
#include "Geometry.h"

Geometry::Geometry() {}
Geometry::Geometry(Material mat, Matrix4 w2o, Matrix4 o2w)
  : worldToObject(w2o), objectToWorld(o2w) {
  material = mat;
}

Triangle::Triangle() {}
Triangle::Triangle(Vector3 aa, Vector3 bb, Vector3 cc,
  Material mat, Matrix4 w2o, Matrix4 o2w)
  : a(aa), b(bb), c(cc), normal((bb - aa).cross(cc - aa).normalized()),
    Geometry(mat, w2o, o2w) {}
Ray Triangle::intersect(const Ray& ray) {
  Ray xfRay = worldToObject.transform(ray);
  // Determine t.
  double d = normal.dot(a);
  double t = (d - normal.dot(xfRay.point)) / normal.dot(xfRay.dir);
  if (t < 0) return {NAN_VECTOR, NAN_VECTOR};
  // Determine if intersection point is within the triangle.
  Vector3 p = xfRay.point + t * xfRay.dir;
  Vector3 pa = p - a;
  Vector3 pb = p - b;
  Vector3 pc = p - c;
  if ((b - a).cross(pa).dot(normal) > 0 &&
      (c - b).cross(pb).dot(normal) > 0 &&
      (a - c).cross(pc).dot(normal) > 0) {
    Vector3 worldPoint =
      objectToWorld.dot(Vector4(p, 1)).toVector3();
    Vector3 worldNormal =
      worldToObject.transposed().dot(Vector4(normal, 0)).toVector3();
    return {worldPoint, worldNormal.normalized()};
  } else {
    return {NAN_VECTOR, NAN_VECTOR};
  }
}

Sphere::Sphere() {}
Sphere::Sphere(Vector3 c, double r, Material mat, Matrix4 w2o, Matrix4 o2w)
  : center(c), radius(r), Geometry(mat, w2o, o2w) {}
Ray Sphere::intersect(const Ray& ray) {
  // Transform ray to the coordinate space of the sphere.
  Ray xfRay = worldToObject.transform(ray);
  xfRay.dir = xfRay.dir.normalized();

  // Calculate quadratic equation coefficients.
  Vector3 x = xfRay.point - center;
  double a = xfRay.dir.dot(xfRay.dir);
  double b = 2 * x.dot(xfRay.dir);
  double c = x.dot(x) - radius*radius;

  // Calculate discriminant to determine number of solutions.
  double discriminant = b*b - 4*a*c;
  // If there are no real solutions, we just return undefined.
  if (discriminant < 0) return {NAN_VECTOR, NAN_VECTOR};

  // Plugging t back into R(t), we find the solutions.
  double tPlus = (-b + std::sqrt(discriminant)) / (2*a);
  double tMinus = (-b - std::sqrt(discriminant)) / (2*a);
  Vector3 solnPlus = xfRay.point + tPlus * xfRay.dir;
  Vector3 solnMinus = xfRay.point + tMinus * xfRay.dir;

  // We ensure the intersection is in front of the ray by checking that t > 0.
  Ray surfaceNormal;
  if (tPlus <= 0 && tMinus <= 0) {
    surfaceNormal = {NAN_VECTOR, NAN_VECTOR};
  } else if (tPlus <= 0 && tMinus > 0) {
    surfaceNormal = {solnMinus, (solnMinus - center)};
  } else if (tPlus > 0 && tMinus <= 0) {
    surfaceNormal = {solnPlus, (solnPlus - center)};
  } else {
    // The only other possibility is that both solutions have positive t-values.
    // We choose the solution that is closer to the ray's start point.
    double distancePlus = (solnPlus - xfRay.point).magnitude();
    double distanceMinus = (solnMinus - xfRay.point).magnitude();
    if (distancePlus < distanceMinus)
      surfaceNormal = {solnPlus, (solnPlus - center)};
    else
      surfaceNormal = {solnMinus, (solnMinus - center)};
  }
  Ray result = objectToWorld.transform(surfaceNormal);
  Vector3 worldPoint =
    objectToWorld.dot(Vector4(surfaceNormal.point, 1)).toVector3();
  Vector3 worldNormal =
    worldToObject.transposed().dot(Vector4(surfaceNormal.dir, 0)).toVector3();
  Ray worldSurfaceNormal = {worldPoint, worldNormal};
  return worldSurfaceNormal;
}
