#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "Sphere.h"
#include "Triangle.h"
#include "Light.h"
#include "Camera.h"
#include "Matrix.h"
#include "Ray.h"
#include "Material.h"

class Scene {
  public:
    std::vector<Sphere> spheres;
    std::vector<Triangle> triangles;
    std::vector<PointLight> pointLights;
    std::vector<DirectionalLight> directionalLights;
    std::vector<AmbientLight> ambientLights;
    Camera camera;

    Matrix4 xfIn;
    Matrix4 xfOut;

    Scene ();
    Scene (std::string s);

    void parseLine (std::string line);
    void simulate ();
    Ray intersect (Ray ray, Sphere s);

    /* Shading */
    Color phong(const Vector3& p, const Vector3& n, const Vector3& v, const Material& material);
    Color ambient (const Color& ka);
    Color diffuse(const Vector3& p, const Vector3& n, const Color& kd);
    Color specular(const Vector3& p, const Vector3& n, const Vector3& v, const Color& ks, double sp);
    double specularIncidence(const Vector3& p, const Vector3& n, const Vector3& v, const Vector3& l, double sp);
};

#endif
