/** Copyright 2016 Alex Yang */
#include <random>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <cstdio>

#include "Scene.h"
#include <pngwriter.h>

Scene::Scene(int res, int aa) {
  resolution = res;
  antialias = aa;
  xfIn = scale(1, 1, 1);
  xfOut = scale(1, 1, 1);
  material =
    {Color(.5, .5, .5), Color(0, 0, 0), Color(0, 0, 0), 1, Color(0, 0, 0)};
}

int Scene::getWidth() {
  Vector3 imagePlaneX = camera.br - camera.bl;
  double imagePlaneW = imagePlaneX.magnitude();
  return static_cast<int>(resolution * imagePlaneW);
}
int Scene::getHeight() {
  Vector3 imagePlaneY = camera.tl - camera.bl;
  double imagePlaneH = imagePlaneY.magnitude();
  return static_cast<int>(resolution * imagePlaneH);
}

void Scene::parseLine(std::string line) {
  if (line.empty()) return;
  std::istringstream iss(line);
  std::string prefix;
  iss >> prefix;

  if (prefix[0] == '#') return;
  
  if (prefix == "cam") {
    Vector3 pts[5];
    for (int i = 0; i < 5; i++)
      iss >> pts[i].x >> pts[i].y >> pts[i].z;
    camera = Camera(pts[0], pts[1], pts[2], pts[3], pts[4]);
  } else if (prefix == "tri") {
    double x1, y1, z1, x2, y2, z2, x3, y3, z3;
    iss >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3;
    objects.emplace_back(
      new Triangle(Vector3(x1, y1, z1), Vector3(x2, y2, z2),
                   Vector3(x3, y3, z3), material, xfIn, xfOut));
  } else if (prefix == "sph") {
    double cx, cy, cz, r;
    iss >> cx >> cy >> cz >> r;
    objects.emplace_back(
      new Sphere(Vector3(cx, cy, cz), r, material, xfIn, xfOut));
  } else if (prefix == "obj") {
    std::string filename;
    iss >> filename;
    parseObj(filename.substr(1, filename.size()-2));
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
  } else if (prefix == "lta") {
    double r, g, b;
    iss >> r >> g >> b;
    ambientLight = Color(r, g, b);
  } else if (prefix == "ltp") {
    double x, y, z, r, g, b, falloff;
    iss >> x >> y >> z >> r >> g >> b >> falloff;
    lights.emplace_back(new PointLight(Vector3(x, y, z), Color(r, g, b)));
  } else if (prefix == "ltd") {
    double x, y, z, r, g, b;
    iss >> x >> y >> z >> r >> g >> b;
    lights.emplace_back(new DirectionalLight(Vector3(x, y, z), Color(r, g, b)));
  } else if (prefix == "mat") {
    double kar, kag, kab, kdr, kdg, kdb, ksr, ksg, ksb, ksp, krr, krg, krb;
    iss >> kar >> kag >> kab
        >> kdr >> kdg >> kdb
        >> ksr >> ksg >> ksb >> ksp
        >> krr >> krg >> krb;
    material = {
      Color(kar, kag, kab),
      Color(kdr, kdg, kdb),
      Color(ksr, ksg, ksb), ksp,
      Color(krr, krg, krb)
    };
  } else {
    std::cerr << "Warning: skipping unrecognized command \""
              << prefix
              << "\" in line:\n  > "
              << line
              << std::endl;
  }
}

void Scene::parseObj(std::string filename) {
  std::ifstream infile(filename);
  std::string line;
  std::vector<Vector3> vertices;
  int numVertices = 0;
  int numFaces = 0;
  while (std::getline(infile, line)) {
    std::string prefix;
    std::istringstream iss(line);
    iss >> prefix;
    if (prefix.compare("v") == 0) {
      double x, y, z;
      iss >> x >> y >> z;
      vertices.push_back(Vector3(x, y, z));
      numVertices++;
    } else if (prefix.compare("f") == 0) {
      int a, b, c;
      iss >> a >> b >> c;
      objects.emplace_back(
        new Triangle(vertices[a-1], vertices[b-1], vertices[c-1],
                     material, xfIn, xfOut));
      numFaces++;
    }
  }
  printf("[OBJ] \"%s\" contained %d vertices and %d faces.\n",
         filename.c_str(), numVertices, numFaces);
}

std::vector<Color> Scene::render() {
  // Determine pixel location from the image plane.
  Vector3 imagePlaneY = camera.tl - camera.bl;
  Vector3 imagePlaneX = camera.br - camera.bl;
  double imagePlaneH = imagePlaneY.magnitude();
  double imagePlaneW = imagePlaneX.magnitude();
  int height = static_cast<int>(resolution * imagePlaneH);
  int width = static_cast<int>(resolution * imagePlaneW);
  Vector3 unitY = imagePlaneY.normalized();
  Vector3 unitX = imagePlaneX.normalized();

  printf("[RENDER] Preparing %dx%d image.\n", width, height);

  // Initialize frame buffer.
  std::vector<Color> frame(width*height);

  // Find rays from the pixel locations.
  double start = profiler.now();
  for (int y = 0; y < height; y++) {
    if (height > 20 && y % (height / 20) == 0) {
      printf("[ %3.0f%% ] %.3f s\n",
        static_cast<double>(y) / height * 100,
        profiler.now() - start);
    }
    for (int x = 0; x < width; x++) {
      // Determine world coordinates of pixel at (x, y) of image plane.
      Vector3 worldPixel = camera.bl
        + unitY * (static_cast<double>(y) / resolution)
        + unitX * (static_cast<double>(x) / resolution);
      std::vector<std::pair<double, double>> samples = jitteredGrid(antialias);
      for (std::pair<double, double> pt : samples) {
        Vector3 worldPoint = worldPixel
          + unitX * (pt.first / resolution)
          + unitY * (pt.second / resolution);
        Vector3 direction = worldPoint - camera.e;
        frame[y*width+x] = frame[y*width+x] + trace({camera.e, direction});
      }
      frame[y*width+x] = frame[y*width+x] * (1.0 / samples.size());
    }
  }
  printf("[RENDER] Completed in %.3f seconds.\n", profiler.now() - start);

  return frame;
}

std::vector<std::pair<double, double>> Scene::jitteredGrid(int size) {
  std::vector<std::pair<double, double>> res;
  if (size == 1) {
    res.push_back(std::pair<double, double>(0.5, 0.5));
    return res;
  }
  std::vector<double> increments;
  for (int i = 0; i < size; i++) {
    increments.push_back(static_cast<double>(i) / size);
  }
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      double jitterX = (static_cast<double>(std::rand()) / RAND_MAX) / size;
      double jitterY = (static_cast<double>(std::rand()) / RAND_MAX) / size;
      double x = increments[i] + jitterX;
      double y = increments[j] + jitterY;
      res.push_back(std::pair<double, double>(x, y));
    }
  }
  return res;
}

Color Scene::trace(const Ray& ray) {
  return trace(ray, kBounceLimit);
}
Color Scene::trace(const Ray& ray, int bouncesLeft) {
  if (bouncesLeft < 0) {
    return kBackgroundColor;
  }
  double nearestDistance = INFINITY;
  Ray nearestIntersection = {NAN_VECTOR, NAN_VECTOR};
  Material nearestMaterial;
  for (int i = 0; i < objects.size(); i++) {
    Geometry& geometry = *objects[i];
    Ray intersection = geometry.intersect(ray);
    if (intersection.point.isDefined()) {
      double distance = (intersection.point - ray.point).magnitude();
      if (distance < nearestDistance && distance > 1e-6) {
        nearestIntersection = intersection;
        nearestDistance = distance;
        nearestMaterial = geometry.material;
      }
    }
  }

  if (!nearestIntersection.point.isDefined()) {
    return kBackgroundColor;
  }

  Vector3 p = nearestIntersection.point;
  Vector3 n = nearestIntersection.dir.normalized();
  Vector3 v = (ray.point - p).normalized();
  Material mat = nearestMaterial;
  Color result = ambient(mat.ka);
  for (int i = 0; i < lights.size(); i++) {
    Light& light = *lights[i];
    Vector3 l = light.dirToLight(p);
    Ray shadowRay = {p, l};
    // Check if there are any intersections between this point and the light.
    bool isShadowed = false;
    for (int i = 0; i < objects.size(); i++) {
      Geometry& geometry = *objects[i];
      Ray intersection = geometry.intersect(shadowRay);
      if (intersection.point.isDefined()) {
        double distanceToIntersection = (intersection.point - p).magnitude();
        // If the intersection lies between the light and the point,
        // skip shading for this light.
        // The second condition prevents self-shadowing.
        if (distanceToIntersection < light.distanceToLight(p) &&
            distanceToIntersection > 1e-6) {
          isShadowed = true;
        }
      }
    }
    if (!isShadowed) {
      result = result + diffuse(p, n, l, mat.kd, light.intensity);
      result = result + specular(p, n, v, l, mat.ks, mat.sp, light.intensity);
    }
  }
  // Recursively trace reflective rays.
  Vector3 reflectedDir = (2 * n) - v;
  result = result + mat.kr * trace({p, reflectedDir}, bouncesLeft-1);
  return result;
}

Color Scene::ambient(const Color& ka) {
  return ka * ambientLight;
}
Color Scene::diffuse(const Vector3& p, const Vector3& n, const Vector3& l,
  const Color& kd, const Color& intensity) {
  return kd * intensity * std::max(0.0, l.dot(n));
}
Color Scene::specular(const Vector3& p, const Vector3& n, const Vector3& v,
  const Vector3& l, const Color& ks, double sp, const Color& intensity) {
  return ks * intensity * specularIncidence(p, n, v, l, sp);
}
double Scene::specularIncidence(const Vector3& p, const Vector3& n,
  const Vector3& v, const Vector3& l, double sp) {
  Vector3 r = (-1.0 * l + 2.0 * l.dot(n) * n).normalized();
  Vector3 h = (l + v).normalized();
  Vector3 hProj = (h - n * h.dot(n)).normalized();
  return std::pow(std::max(0.0, r.dot(v)), sp);
}
