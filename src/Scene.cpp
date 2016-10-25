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
}
Scene::Scene (std::string s) {
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
    float cx, cy, cz, r;
    iss >> cx >> cy >> cz >> r;
    // Construct Sphere object in this stack frame, then copy it into vector.
    this->spheres.push_back(Sphere(cx, cy, cz, r));
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
      for (Sphere s : spheres) {
        SurfacePoint sp = intersect(camera.e, direction, s);
        if (sp.point.isDefined()) {
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

SurfacePoint Scene::intersect (const Vector3 start, const Vector3 direction,
  const Sphere s) {
  // Let R(t) := A + tD, C := sphere center, r := sphere radius, X := A - C
  // 0 = | A + tD - C |^2 - r^2
  // 0 = | X + tD |^2 - r^2
  // 0 = (X_1 + tD_1)^2 + (X_2 + tD_2)^2 + (X_3 + tD_3)^2 - r^2
  // 0 = X_1^2 + 2tX_1D_1 + t^2D_1^2 + X_2^2 + 2tX_2D_2 + t^2D_2^2 + ... - r^2 = 0
  // 0 = |X|^2 - r^2 + 2t(X.D) + t^2|D|^2

  // Calculate quadratic equation coefficients.
  Vector3 x = start - s.center;
  double a = x.dot(x) - s.radius*s.radius;
  double b = 2 * x.dot(direction);
  double c = direction.dot(direction);

  // Calculate discriminant to determine number of solutions.
  double discriminant = b*b - 4*a*c;
  // If there are no real solutions, we just return undefined.
  if (discriminant < 0) return {NAN_VECTOR, NAN_VECTOR};

  // Plugging t back into R(t), we find the solutions.
  double tPlus = (-b + sqrt(discriminant)) / (2*a);
  double tMinus = (-b - sqrt(discriminant)) / (2*a);
  Vector3 solnPlus = start + tPlus * direction;
  Vector3 solnMinus = start + tMinus * direction;

  // We ensure the intersection is in front of the ray by checking that t > 0.
  if (tPlus <= 0 && tMinus <= 0) return {NAN_VECTOR, NAN_VECTOR};
  if (tPlus <= 0 && tMinus > 0) return {solnMinus, (solnMinus - s.center).normalized()};
  if (tPlus > 0 && tMinus <= 0) return {solnPlus, (solnPlus - s.center).normalized()};

  // The only other possibility is that both solutions have positive t-values.
  // We choose the solution that is closer to the ray's start point.
  double distancePlus = (solnPlus - start).magnitude();
  double distanceMinus = (solnMinus - start).magnitude();
  if (distancePlus < distanceMinus)
    return {solnPlus, (solnPlus - s.center).normalized()};
  else
    return {solnMinus, (solnMinus - s.center).normalized()};
}
