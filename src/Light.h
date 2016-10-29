#ifndef LIGHT_H
#define LIGHT_H

#include "Vector.h"
#include "Color.h"

class Light {
 public:
  Color intensity;

  Light();
  Light(Color intensity);
  virtual Vector3 dirToLight(const Vector3& p) = 0;
  virtual double distanceToLight (const Vector3& p) = 0;
};

class PointLight : public Light {
  public:
    Vector3 point;

    PointLight ();
    PointLight (Vector3 p, Color i);
    Vector3 dirToLight (const Vector3& p) override;
    double distanceToLight (const Vector3& p) override;
};

class DirectionalLight : public Light {
  public:
    Vector3 dir;

    DirectionalLight ();
    DirectionalLight (Vector3 d, Color i);
    Vector3 dirToLight (const Vector3& p) override;
    double distanceToLight (const Vector3& p) override;
};

class AmbientLight {};
#endif
