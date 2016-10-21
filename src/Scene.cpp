#include <iostream>
#include <string>
#include <sstream>

#include "Scene.h"

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
  std::string s = line.substr(0, 3); 
  std::cout << s << std::endl;
}
