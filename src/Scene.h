#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Sphere.h"
#include "Triangle.h"
#include "Light.h"
#include "Camera.h"

class Scene {
  public:
    std::vector<Sphere> spheres;
    std::vector<Triangle> triangles;
    std::vector<PointLight> pointLights;
    std::vector<DirectionalLight> directionalLights;
    std::vector<AmbientLight> ambientLights;
    Camera camera;

    Scene ();
    Scene (std::string s);

    void parseLine (std::string line);
    void simulate ();
    Vector3 intersect (const Vector3 start, const Vector3 direction, const Sphere s);
};

#endif
