/** Copyright 2016 Alex Yang */
#include <cmath>
#include "Light.h"

Light::Light() {}
Light::Light(Color i) : intensity(i) {}

PointLight::PointLight() {}
PointLight::PointLight(Vector3 p, Color i) : point(p), Light(i) { }
Vector3 PointLight::dirToLight(const Vector3& p) {
  return (point - p).normalized();
}
double PointLight::distanceToLight(const Vector3& p) {
  return (point - p).magnitude();
}

DirectionalLight::DirectionalLight() {}
DirectionalLight::DirectionalLight(Vector3 d, Color i) : dir(d), Light(i) {}
Vector3 DirectionalLight::dirToLight(const Vector3& p) {
  return (-1 * dir).normalized();
}
double DirectionalLight::distanceToLight(const Vector3& p) {
  return INFINITY;
}
