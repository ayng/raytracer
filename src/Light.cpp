#include "Light.h"
#include "Vector.h"
#include "Color.h"

PointLight::PointLight () {}
PointLight::PointLight (Vector3 p, Color c) : point(p), intensity(c) { }
Vector3 PointLight::dirToLight (Vector3 p) {
  return point - p;
}

DirectionalLight::DirectionalLight () {}
DirectionalLight::DirectionalLight (Vector3 d, Color c) : dir(d), intensity(c) { }
Vector3 DirectionalLight::dirToLight (Vector3 p) {
  return -1 * dir;
}
