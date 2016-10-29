/** Copyright 2016 Alex Yang */
#include <pngwriter.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>
#include <cstdio>

#include "Scene.h"

Scene::Scene() {
  xfIn = scale(1, 1, 1);
  xfOut = scale(1, 1, 1);
  material =
    {Color(.5, .5, .5), Color(0, 0, 0), Color(0, 0, 0), 1, Color(0, 0, 0)};
}

void Scene::parseLine(std::string line) {
  if (line.empty()) return;
  std::istringstream iss(line);
  std::string prefix;
  iss >> prefix;
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

void Scene::render() {
  // Determine pixel location from the image plane.
  Vector3 imagePlaneY = camera.tl - camera.bl;
  Vector3 imagePlaneX = camera.br - camera.bl;
  double imagePlaneH = imagePlaneY.magnitude();
  double imagePlaneW = imagePlaneX.magnitude();
  int height = static_cast<int>(kResolution * imagePlaneH);
  int width = static_cast<int>(kResolution * imagePlaneW);
  Vector3 unitY = imagePlaneY.normalized();
  Vector3 unitX = imagePlaneX.normalized();

  printf("[RENDER] Writing %dx%d image.\n", width, height);

  // Initialize frame buffer.
  Color *frame = new Color[width*height];

  // Find rays from the pixel locations.
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Determine world coordinates of pixel at (x, y) of image plane.
      Vector3 world = camera.bl
        + unitY * (static_cast<double>(y) / kResolution)
        + unitX * (static_cast<double>(x) / kResolution);
      // Point ray from camera eye to center of pixel.
      Vector3 direction = (world
        + unitX * (0.5 / kResolution)
        + unitY * (0.5 / kResolution)) - camera.e;
      Ray cameraRay = {camera.e, direction};

      frame[y*width+x] = trace(cameraRay);
    }
  }

  // Save frame buffer to an image.
  pngwriter png(width, height, 0, "output.png");
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Color c = frame[y*width+x];
      png.plot(x+1, y+1, c.r, c.g, c.b);
    }
  }
  png.close();

  delete [] frame;
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
  result = result + trace({p, reflectedDir}, bouncesLeft-1);
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
