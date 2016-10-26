#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cmath>

#include <pngwriter.h>
#include "Color.h"
#include "Scene.h"

const std::map<std::string, int> argnum = {
  {"cam", 15},
  {"sph", 4}
};

Scene::Scene () {
  transform = scale(1, 1, 1);
}
Scene::Scene (std::string s) {
  transform = scale(1, 1, 1);
  std::stringstream ss(s);
  std::string line;
  while (std::getline(ss, line, '\n')) {
    this->parseLine(line);
  }
}

void Scene::parseLine (std::string line) {
  std::istringstream iss(line);
  std::string prefix;
  iss >> prefix;
  if (prefix == "cam") { // Camera
    Vector3 pts[5];
    for (int i = 0; i < 5; i++)
      iss >> pts[i].x >> pts[i].y >> pts[i].z;
    this->camera = Camera(pts[0], pts[1], pts[2], pts[3], pts[4]);
  }
  else if (prefix == "sph") { // Sphere
    double cx, cy, cz, r;
    iss >> cx >> cy >> cz >> r;
    // Construct Sphere object in this stack frame, then copy it into vector.
    this->spheres.push_back(Sphere(cx, cy, cz, r, transform));
  }
  else if (prefix == "xfz") {
    transform = scale(1, 1, 1);
  }
  else if (prefix == "xft") {
    double x, y, z;
    iss >> x >> y >> z;
    transform = translate(-x, -y, -z).dot(transform);
  }
  else if (prefix == "xfr") {
    double x, y, z;
    iss >> x >> y >> z;
    transform = rotate(-x, -y, -z).dot(transform);
  }
  else if (prefix == "xfs") {
    double x, y, z;
    iss >> x >> y >> z;
    transform = scale(1/x, 1/y, 1/z).dot(transform);
  }
  else {
    std::cerr << "Warning: skipping unrecognized command \""
              << prefix
              << "\" in line:\n  > "
              << line
              << std::endl;
  }
}

void Scene::simulate () {
  const int resolution = 100;
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
  Color frame[width][height];

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
      for (Sphere s : spheres) {
        Ray surfacePoint = intersect(cameraRay, s);
        if (surfacePoint.point.isDefined()) {
          frame[x][y] = Color(1,0,0);
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
      Color c = frame[x][y];
      png.plot(x, y, c.r, c.g, c.b);
    }
  }
  png.close();
}

Ray Scene::intersect (const Ray ray, const Sphere sph) {
  // Let R(t) := A + tD, C := sphere center, r := sphere radius, X := A - C
  // 0 = | A + tD - C |^2 - r^2
  // 0 = | X + tD |^2 - r^2
  // 0 = (X_1 + tD_1)^2 + (X_2 + tD_2)^2 + (X_3 + tD_3)^2 - r^2
  // 0 = X_1^2 + 2tX_1D_1 + t^2D_1^2 + X_2^2 + 2tX_2D_2 + t^2D_2^2 + ... - r^2 = 0
  // 0 = |X|^2 - r^2 + 2t(X.D) + t^2|D|^2
  // Transform ray to the coordinate space of the sphere.
  Vector3 tPoint = sph.transform.dot(Vector4(ray.point, 1)).toVector3();
  Vector3 tDir = sph.transform.dot(Vector4(ray.dir, 0)).toVector3();
  Ray xfRay = {tPoint, tDir};

  // Calculate quadratic equation coefficients.
  Vector3 x = xfRay.point - sph.center;
  double a = x.dot(x) - sph.radius*sph.radius;
  double b = 2 * x.dot(xfRay.dir);
  double c = xfRay.dir.dot(xfRay.dir);

  // Calculate discriminant to determine number of solutions.
  double discriminant = b*b - 4*a*c;
  // If there are no real solutions, we just return undefined.
  if (discriminant < 0) return {NAN_VECTOR, NAN_VECTOR};

  // Plugging t back into R(t), we find the solutions.
  double tPlus = (-b + sqrt(discriminant)) / (2*a);
  double tMinus = (-b - sqrt(discriminant)) / (2*a);
  Vector3 solnPlus = xfRay.point + tPlus * xfRay.dir;
  Vector3 solnMinus = xfRay.point + tMinus * xfRay.dir;

  // We ensure the intersection is in front of the ray by checking that t > 0.
  if (tPlus <= 0 && tMinus <= 0) return {NAN_VECTOR, NAN_VECTOR};
  if (tPlus <= 0 && tMinus > 0) return {solnMinus, (solnMinus - sph.center).normalized()};
  if (tPlus > 0 && tMinus <= 0) return {solnPlus, (solnPlus - sph.center).normalized()};

  // The only other possibility is that both solutions have positive t-values.
  // We choose the solution that is closer to the ray's start point.
  double distancePlus = (solnPlus - xfRay.point).magnitude();
  double distanceMinus = (solnMinus - xfRay.point).magnitude();
  if (distancePlus < distanceMinus)
    return {solnPlus, (solnPlus - sph.center).normalized()};
  else
    return {solnMinus, (solnMinus - sph.center).normalized()};
}
