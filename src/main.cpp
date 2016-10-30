#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdio>
#include <sys/time.h>

#include <pngwriter.h>
#include <docopt.h>
#include "Scene.h"

static const char kUsage[] =
R"(Raytracer.

Usage: raytracer [options]

Options:
  -o FILE     Output file name.
  -r INTEGER  Resolution in pixels per unit [default: 40].
)";

int main(int argc, char **argv) {
  // Configure with command line arguments.
  std::map<std::string, docopt::value> args = docopt::docopt(kUsage,
                       { argv + 1, argv + argc },
                       true,               // show help if requested
                       "Raytracer 0.0");  // version string

  std::string filename = args["-o"].asString();
  long res = args["-r"].asLong();

  Scene scene(res, filename);

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
