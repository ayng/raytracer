/** Copyright 2016 Alex Yang */
#include <pngwriter.h>

#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include <cmath>
#include <cstdio>

#include "Color.h"
#include "Scene.h"
#include "Material.h"

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
  if (prefix == "cam") {  // Camera
    Vector3 pts[5];
    for (int i = 0; i < 5; i++)
      iss >> pts[i].x >> pts[i].y >> pts[i].z;
    this->camera = Camera(pts[0], pts[1], pts[2], pts[3], pts[4]);
  } else if (prefix == "sph") {  // Sphere
    double cx, cy, cz, r;
    iss >> cx >> cy >> cz >> r;
    this->spheres.push_back(Sphere(cx, cy, cz, r, material, xfIn, xfOut));
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
  Sphere nearestSphere;
  for (Sphere sph : spheres) {
    Ray intersection = intersect(ray, sph);
    if (intersection.point.isDefined()) {
      double distance = (intersection.point - ray.point).magnitude();
      if (distance < nearestDistance && distance > 1e-6) {
        nearestIntersection = intersection;
        nearestDistance = distance;
        nearestSphere = sph;
      }
    }
  }

  if (!nearestIntersection.point.isDefined()) {
    return kBackgroundColor;
  }

  Vector3 p = nearestIntersection.point;
  Vector3 n = nearestIntersection.dir.normalized();
  Vector3 v = (ray.point - p).normalized();
  Material mat = nearestSphere.material;
  Color result = ambient(mat.ka);
  for (int i = 0; i < lights.size(); i++) {
    Light& light = *lights[i];
    Vector3 l = light.dirToLight(p);
    Ray shadowRay = {p, l};
    // Check if there are any intersections between this point and the light.
    bool isShadowed = false;
    for (Sphere sph : spheres) {
      Ray intersection = intersect(shadowRay, sph);
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


Ray Scene::intersect(const Ray ray, const Sphere sph) {
  // Transform ray to the coordinate space of the sphere.
  Ray xfRay = sph.in.transform(ray);
  xfRay.dir = xfRay.dir.normalized();

  // Calculate quadratic equation coefficients.
  Vector3 x = xfRay.point - sph.center;
  double a = xfRay.dir.dot(xfRay.dir);
  double b = 2 * x.dot(xfRay.dir);
  double c = x.dot(x) - sph.radius*sph.radius;

  // Calculate discriminant to determine number of solutions.
  double discriminant = b*b - 4*a*c;
  // If there are no real solutions, we just return undefined.
  if (discriminant < 0) return {NAN_VECTOR, NAN_VECTOR};

  // Plugging t back into R(t), we find the solutions.
  double tPlus = (-b + std::sqrt(discriminant)) / (2*a);
  double tMinus = (-b - std::sqrt(discriminant)) / (2*a);
  Vector3 solnPlus = xfRay.point + tPlus * xfRay.dir;
  Vector3 solnMinus = xfRay.point + tMinus * xfRay.dir;

  // We ensure the intersection is in front of the ray by checking that t > 0.
  Ray surfaceNormal;
  if (tPlus <= 0 && tMinus <= 0) {
    surfaceNormal = {NAN_VECTOR, NAN_VECTOR};
  } else if (tPlus <= 0 && tMinus > 0) {
    surfaceNormal = {solnMinus, (solnMinus - sph.center)};
  } else if (tPlus > 0 && tMinus <= 0) {
    surfaceNormal = {solnPlus, (solnPlus - sph.center)};
  } else {
    // The only other possibility is that both solutions have positive t-values.
    // We choose the solution that is closer to the ray's start point.
    double distancePlus = (solnPlus - xfRay.point).magnitude();
    double distanceMinus = (solnMinus - xfRay.point).magnitude();
    if (distancePlus < distanceMinus)
      surfaceNormal = {solnPlus, (solnPlus - sph.center)};
    else
      surfaceNormal = {solnMinus, (solnMinus - sph.center)};
  }
  Ray result = sph.out.transform(surfaceNormal);
  Vector3 worldPoint = sph.out.dot(Vector4(surfaceNormal.point, 1)).toVector3();
  Vector3 worldNormal =
    sph.in.transposed().dot(Vector4(surfaceNormal.dir, 0)).toVector3();
  Ray worldSurfaceNormal = {worldPoint, worldNormal};
  return worldSurfaceNormal;
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
