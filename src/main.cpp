#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdio>
#include <sys/time.h>

#include <pngwriter.h>
#include "Scene.h"

int main(int argc, char **argv) {
  Scene scene;
  // Read scene description from stdin.
  for (std::string line; std::getline(std::cin, line);) {
    scene.parseLine(line);
  }
  // Time the render.
  scene.render();

  pngwriter png(100, 100, 0, "test.png");
  for (int i = 0; i < 100; i++)
    png.plot(10, i, 1.0, 1.0, 1.0);
  png.plot(40, 50, 0.0, 0.0, 1.0);
  png.plot(50, 50, 1.0, 0.0, 0.0);
  png.plot(60, 50, 0.0, 1.0, 0.0);
  png.close();
}
