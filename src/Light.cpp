#include "Light.h"
#include "Vector.h"
#include "Color.h"

PointLight::PointLight () {}
PointLight::PointLight (Vector3 p, Color c) : point(p), intensity(c) { }
