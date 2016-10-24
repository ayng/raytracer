#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdio>

#include <pngwriter.h>
#include "Color.h"
#include "Scene.h"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Please pass in a scene description file." << std::endl;
    exit(1);
  }

  // Read file contents into string.
  std::ifstream in(argv[1]);
  in.seekg(0, std::ios::end);
  int length = in.tellg();
  std::string sceneDescription;
  sceneDescription.resize(length);
  in.seekg(0, std::ios::beg);
  in.read(&sceneDescription[0], length);

  // Construct the scene from its description.
  Scene scene(sceneDescription);
  scene.simulate();

  pngwriter png(100, 100, 0, "test.png");
  for (int i = 0; i < 100; i++)
    png.plot(10, i, 1.0, 1.0, 1.0);
  png.plot(40, 50, 0.0, 0.0, 1.0);
  png.plot(50, 50, 1.0, 0.0, 0.0);
  png.plot(60, 50, 0.0, 1.0, 0.0);
  png.close();
}
