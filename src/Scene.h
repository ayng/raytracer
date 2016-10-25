#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Sphere.h"
#include "Triangle.h"
#include "Light.h"
#include "Camera.h"
#include "Matrix.h"

struct Ray {
  Vector3 point;
  Vector3 dir;
};

class Scene {
  public:
    std::vector<Sphere> spheres;
    std::vector<Triangle> triangles;
    std::vector<PointLight> pointLights;
    std::vector<DirectionalLight> directionalLights;
    std::vector<AmbientLight> ambientLights;
    Camera camera;

    Matrix4 transform;

    Scene ();
    Scene (std::string s);

    void parseLine (std::string line);
    void simulate ();
    Ray intersect (const Ray ray, const Sphere s);
};

#endif
