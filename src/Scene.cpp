#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include "Scene.h"

const std::map<std::string, int> argnum = {
  {"cam", 15},
  {"sph", 4}
};

Scene::Scene() {
}
Scene::Scene(std::string s) {
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
  if (prefix == "cam") { // Camera
    Vector3 pts[5];
    for (int i = 0; i < 5; i++)
      iss >> pts[i].x >> pts[i].y >> pts[i].z;
    this->camera.set(pts[0], pts[1], pts[2], pts[3], pts[4]);
  }
  else if (prefix == "sph") { // Sphere
    float cx, cy, cz, r;
    iss >> cx >> cy >> cz >> r;
    // Construct Sphere object in this stack frame, then copy it into vector.
    this->spheres.push_back(Sphere(cx, cy, cz, r));
    this->spheres[0].center.dump();
  }
  else {
    std::cerr << "Warning: skipping unrecognized command \""
              << prefix
              << "\" in line:\n  > "
              << line
              << std::endl;
  }
}
