#include <pngwriter.h>

#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include <cmath>

#include "Color.h"
#include "Scene.h"
#include "Material.h"


const std::map<std::string, int> argnum = {
  {"cam", 15},
  {"sph", 4}
};

Scene::Scene() {
  xfIn = scale(1, 1, 1);
  xfOut = scale(1, 1, 1);
}
Scene::Scene(std::string s) {
  xfIn = scale(1, 1, 1);
  xfOut = scale(1, 1, 1);
  std::stringstream ss(s);
  std::string line;
  while (std::getline(ss, line, '\n')) {
    this->parseLine(line);
  }
}

void Scene::parseLine(std::string line) {
  std::istringstream iss(line);
  std::string prefix;
  iss >> prefix;
  if (prefix == "cam") {  // Camera
    Vector3 pts[5];
    for (int i = 0; i < 5; i++)
      iss >> pts[i].x >> pts[i].y >> pts[i].z;
    this->camera = Camera(pts[0], pts[1], pts[2], pts[3], pts[4]);
  } else if (prefix == "sph") {  // Sphere
    double cx, cy, cz, r;
    iss >> cx >> cy >> cz >> r;
    // Construct Sphere object in this stack frame, then copy it into vector.
    this->spheres.push_back(Sphere(cx, cy, cz, r, xfIn, xfOut));
  } else if (prefix == "xfz") {
    xfIn = scale(1, 1, 1);
    xfOut = scale(1, 1, 1);
  } else if (prefix == "xft") {
    double x, y, z;
    iss >> x >> y >> z;
    xfIn = translate(-x, -y, -z).dot(xfIn);
    xfOut = xfOut.dot(translate(x, y, z));
  } else if (prefix == "xfr") {
    double x, y, z;
    iss >> x >> y >> z;
    xfIn = rotate(-x, -y, -z).dot(xfIn);
    xfOut = xfOut.dot(rotate(x, y, z));
  } else if (prefix == "xfs") {
    double x, y, z;
    iss >> x >> y >> z;
    xfIn = scale(1/x, 1/y, 1/z).dot(xfIn);
    xfOut = xfOut.dot(scale(x, y, z));
  } else if (prefix == "ltp") {
    double x, y, z, r, g, b, falloff;
    iss >> x >> y >> z >> r >> g >> b >> falloff;
    pointLights.push_back(PointLight(Vector3(x, y, z), Color(r, g, b)));
  } else {
    std::cerr << "Warning: skipping unrecognized command \""
              << prefix
              << "\" in line:\n  > "
              << line
              << std::endl;
  }
}

void Scene::simulate() {
  const int resolution = 400;
  // Determine pixel location from the image plane.
  Vector3 imagePlaneY = camera.tl - camera.bl;
  Vector3 imagePlaneX = camera.br - camera.bl;
  double imagePlaneH = imagePlaneY.magnitude();
  double imagePlaneW = imagePlaneX.magnitude();
  int height = (int) (resolution * imagePlaneH);
  int width = (int) (resolution * imagePlaneW);
  Vector3 unitY = imagePlaneY.normalized();
  Vector3 unitX = imagePlaneX.normalized();

  // Initialize frame buffer.
  Color *frame = new Color[width*height];

  int hitCount = 0;
  int total = 0;
  // Find rays from the pixel locations.
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Determine world coordinates of pixel at (x, y) of image plane.
      Vector3 world = camera.bl
        + unitY * ((double) y / resolution)
        + unitX * ((double) x / resolution);
      // Point ray from camera eye to center of pixel.
      Vector3 direction = (world
        + unitX * (0.5 / resolution)
        + unitY * (0.5 / resolution)) - camera.e;
      Ray cameraRay = {camera.e, direction};

      Color ka(.2, .2, .2);
      Color kd(.8, .8, .1);
      Color ks(.5, .5, .1);
      double sp = 100;
      Material material = {ka, kd, ks, sp};
      for (Sphere sph : spheres) {
        Ray surfaceNormal = intersect(cameraRay, sph);
        if (surfaceNormal.point.isDefined()) {
          Vector3 point = surfaceNormal.point;
          Vector3 normalDir = surfaceNormal.dir;
          Vector3 viewDir = (camera.e - point).normalized();
          frame[y*width+x] = phong(point, normalDir, viewDir, material);
          hitCount++;
        }
      }
      total++;
    }
  }
  std::cout << "out of " << total << " rays traced, " << hitCount << " hit." << std::endl;

  // Save frame buffer to an image.
  pngwriter png(width, height, 0, "output.png");
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Color c = frame[y*width+x];
      png.plot(x, y, c.r, c.g, c.b);
    }
  }
  png.close();

  delete [] frame;
}

Ray Scene::intersect(const Ray ray, const Sphere sph) {
  // Let R(t) := A + tD, C := sphere center, r := sphere radius, X := A - C
  // 0 = | A + tD - C |^2 - r^2
  // 0 = | X + tD |^2 - r^2
  // 0 = (X_1 + tD_1)^2 + (X_2 + tD_2)^2 + (X_3 + tD_3)^2 - r^2
  // 0 = X_1^2 + 2tX_1D_1 + t^2D_1^2 + X_2^2 + 2tX_2D_2 + t^2D_2^2 + ... - r^2 = 0
  // 0 = |X|^2 - r^2 + 2t(X.D) + t^2|D|^2
  // Transform ray to the coordinate space of the sphere.
  Ray xfRay = sph.in.transform(ray);

  // Calculate quadratic equation coefficients.
  Vector3 x = xfRay.point - sph.center;
  double a = xfRay.dir.dot(xfRay.dir);
  double b = 2 * x.dot(xfRay.dir);
  double c = x.dot(x) - sph.radius*sph.radius;

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
    surfaceNormal = {solnMinus, (solnMinus - sph.center)};
  } else if (tPlus > 0 && tMinus <= 0) {
    surfaceNormal = {solnPlus, (solnPlus - sph.center)};
  } else {
    // The only other possibility is that both solutions have positive t-values.
    // We choose the solution that is closer to the ray's start point.
    double distancePlus = (solnPlus - xfRay.point).magnitude();
    double distanceMinus = (solnMinus - xfRay.point).magnitude();
    if (distancePlus < distanceMinus)
      surfaceNormal = {solnPlus, (solnPlus - sph.center)};
    else
      surfaceNormal = {solnMinus, (solnMinus - sph.center)};
  }
  Ray result = sph.out.transform(surfaceNormal);
  result.dir = result.dir.normalized();
  return result;
}

Color Scene::phong(const Vector3& p, const Vector3& n, const Vector3& v, const Material& material) {
  return ambient(material.ka)
       + diffuse(p, n, material.kd)
       + specular(p, n, v, material.ks, material.sp);
}
Color Scene::ambient(const Color& ka) {
  return ka;
}
Color Scene::diffuse(const Vector3& p, const Vector3& n, const Color& kd) {
  Color res;
  for (PointLight pl : pointLights) {
    Vector3 l = (pl.point - p).normalized();
    res = res + pl.intensity * kd * std::max(0.0, l.dot(n));
  }
  for (DirectionalLight dl : directionalLights) {
    Vector3 l = -1 * dl.dir.normalized();
    res = res + dl.intensity * kd * std::max(0.0, l.dot(n));
  }
  return res;
}
double Scene::specularIncidence(const Vector3& p, const Vector3& n, const Vector3& v, const Vector3& l, double sp) {
  Vector3 r = (-1.0 * l + 2.0 * l.dot(n) * n).normalized();
  Vector3 h = (l + v).normalized();
  Vector3 hProj = (h - n * h.dot(n)).normalized();
  return std::pow(std::max(0.0, r.dot(v)), sp);
}
Color Scene::specular(const Vector3& p, const Vector3& n, const Vector3& v, const Color& ks, double sp) {
  Color res;
  for (PointLight pl : pointLights) {
    Vector3 l = (pl.point - p).normalized();
    res = res + ks * pl.intensity * specularIncidence(p, n, v, l, sp);
  }
  for (DirectionalLight dl : directionalLights) {
    Vector3 l = -1 * dl.dir.normalized();
    res = res + ks * dl.intensity * specularIncidence(p, n, v, l, sp);
  }
  return res;
}
