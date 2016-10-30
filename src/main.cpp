#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdio>
#include <sys/time.h>
#include <unistd.h>

#include <pngwriter.h>
#include "Scene.h"

const int kDefaultResolution = 40;
const char kUsage[] = "Usage: raytracer -o myimage.png [-r <resolution>]";

int main(int argc, char **argv) {
  // Configure with command line arguments.
  int resolution = kDefaultResolution;
  char *filename = NULL;
  bool isFilenameSet = false;

  for (int i = 0; i < argc; i++) {
    std::string arg(argv[i]);
    if (arg.compare("-o") == 0) {
      filename = argv[++i];
      isFilenameSet = true;
    }
    if (arg.compare("-r") == 0) {
      resolution = atoi(argv[++i]);
    }
  }

  if (!isFilenameSet) {
    std::cerr << "[ERROR] Filename not set.\n" << kUsage << std::endl;
    return 1;
  }

  Scene scene(resolution);

  // Read scene description from stdin.
  for (std::string line; std::getline(std::cin, line);) {
    scene.parseLine(line);
  }

  std::vector<Color> frame = scene.render();

  // Save frame buffer to an image.
  int width = scene.getWidth();
  int height = scene.getHeight();
  printf("[MAIN] Writing to \"%s\".\n", filename);
  pngwriter png(width, height, 0, filename);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Color c = frame[y*width+x];
      png.plot(x+1, y+1, c.r, c.g, c.b);
    }
  }
  png.close();

  return 0;
}
