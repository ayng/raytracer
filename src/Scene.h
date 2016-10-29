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
#include "Intersection.h"

class Scene {
  public:
    const int resolution = 240;

    std::vector<Sphere> spheres;
    std::vector<Triangle> triangles;
    std::vector<PointLight> pointLights;
    std::vector<DirectionalLight> directionalLights;
    Camera camera;

    Matrix4 xfIn;
    Matrix4 xfOut;

    Material material;

    Scene ();

    void parseLine (std::string line);
    void render ();
    Ray intersect (Ray ray, Sphere s);
    Intersection nearestIntersection (const Ray& ray);

    /* Shading */
    Color phong(const Vector3& p, const Vector3& n, const Vector3& v, const Material& material, int numBounces);
    Color ambient (const Color& ka);
    Color diffuse(const Vector3& p, const Vector3& n, const Vector3& l, const Color& kd, const Color& intensity);
    Color specular(const Vector3& p, const Vector3& n, const Vector3& v, const Vector3& l, const Color& ks, double sp, const Color& intensity);
    double specularIncidence(const Vector3& p, const Vector3& n, const Vector3& v, const Vector3& l, double sp);
    Color reflect(const Vector3& p, const Vector3& n, const Vector3& v, const Color& kr, int numBounces);
};

#endif
