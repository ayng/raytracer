#include <iostream>
#include <sstream>
#include <string>
#include <map>

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
  // Find rays from the pixel locations.
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Determine world coordinates of pixel at (x, y) of image plane.
      Vector3 world = camera.bl
        + unitY * ((double) y / resolution)
        + unitX * ((double) x / resolution);
      // Point ray from camera eye to center of pixel.
      Vector3 direction = (
        world
        + unitX * (0.5 / resolution)
        + unitY * (0.5 / resolution)
      ) - camera.e;
    }
  }
}
