#ifndef LIGHT_H
#define LIGHT_H

#include "Vector.h"
#include "Color.h"

class PointLight {
  public:
    Vector3 point;
    Color intensity;

    PointLight ();
    PointLight (Vector3 p, Color c);
    Vector3 dirToLight (Vector3 p);
};

class DirectionalLight {
  public:
    Vector3 dir;
    Color intensity;

    DirectionalLight ();
    DirectionalLight (Vector3 d, Color c);
    Vector3 dirToLight (Vector3 p);
};

class AmbientLight {};
#endif
