#include <cmath>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <initializer_list>

#include "Matrix.h"

/* Matrix4 */

Matrix4::Matrix4 () {
  for (int i = 0; i < 16; i++) v[i] = 0;
}

Matrix4::Matrix4 (std::initializer_list<double> list) {
  std::copy(list.begin(), list.end(), v);
}

Matrix4::Matrix4 (const Matrix4& orig) {
  std::copy(orig.v, orig.v+16, v);
}

Matrix4 Matrix4::dot (const Matrix4& rhs) const {
  Matrix4 res;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 4; k++) {
        res.v[i*4+j] += v[i*4+k] * rhs.v[k*4+j];
      }
    }
  }
  return res;
}
Vector4 Matrix4::dot (const Vector4& rhs) const {
  Vector4 res;
  int i = 0;
  res.x = rhs.dot(Vector4(v[i*4+0], v[i*4+1], v[i*4+2], v[i*4+3])); i++;
  res.y = rhs.dot(Vector4(v[i*4+0], v[i*4+1], v[i*4+2], v[i*4+3])); i++;
  res.z = rhs.dot(Vector4(v[i*4+0], v[i*4+1], v[i*4+2], v[i*4+3])); i++;
  res.w = rhs.dot(Vector4(v[i*4+0], v[i*4+1], v[i*4+2], v[i*4+3]));
  return res;
}

Ray Matrix4::transform (const Ray& rhs) const {
  Vector3 xfPoint = dot(Vector4(rhs.point, 1)).toVector3();
  Vector3 xfDir = dot(Vector4(rhs.point+rhs.dir, 1)).toVector3() - xfPoint;
  return {xfPoint, xfDir};
}

Matrix4 Matrix4::transposed () const {
  Matrix4 res;
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      res.v[i*4+j] = v[j*4+i];
    }
  }
  return res;
}

void Matrix4::dump () const {
  printf("[[ %.3f, %.3f, %.3f, %.3f ]\n", v[0], v[1], v[2], v[3]);
  printf(" [ %.3f, %.3f, %.3f, %.3f ]\n", v[4], v[5], v[6], v[7]);
  printf(" [ %.3f, %.3f, %.3f, %.3f ]\n", v[8], v[9], v[10], v[11]);
  printf(" [ %.3f, %.3f, %.3f, %.3f ]]\n", v[12], v[13], v[14], v[15]);
}

Matrix4 scale(double x, double y, double z) {
  return Matrix4({
    x, 0, 0, 0,
    0, y, 0, 0,
    0, 0, z, 0,
    0, 0, 0, 1
  });
}
Matrix4 rotate(double x, double y, double z) {
  Vector3 r(x, y, z);
  if (r.magnitude() == 0) return scale(1, 1, 1);
  Vector3 rProjYZ = (r - r.dot(Vector3(1, 0, 0)) * Vector3(1, 0, 0));
  double xAngle;
  if (rProjYZ.magnitude() == 0) {
    xAngle = 0;
  } else {
    xAngle = - std::acos(rProjYZ.normalized().dot(Vector3(0, 1, 0)));
  }

  // Step 1: Rotate about the x-axis until the xy-plane contains vector r.
  Matrix4 rotateX({
    1, 0, 0, 0,
    0, std::cos(xAngle), -std::sin(xAngle), 0,
    0, std::sin(xAngle),  std::cos(xAngle), 0,
    0, 0, 0, 1
  });
  // Create the inverse as well. We'll need this to undo the first step.
  Matrix4 rotateXInv({
    1, 0, 0, 0,
    0, std::cos(-xAngle), -std::sin(-xAngle), 0,
    0, std::sin(-xAngle),  std::cos(-xAngle), 0,
    0, 0, 0, 1
  });
  // Calculate the amount of rotation needed to match the x-axis with r.
  // zAngle = arccos( cos(zAngle) ) = arccos( normalize(R_x . r) . x^hat )
  double zAngle = - std::acos(
    rotateX.dot(Vector4(r, 1)).toVector3().normalized().dot(Vector3(1, 0, 0)));
  // Step 2: Rotate about the the z-axis to match the x-axis with r.
  Matrix4 rotateZ({
    std::cos(zAngle), -std::sin(zAngle), 0, 0,
    std::sin(zAngle),  std::cos(zAngle), 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  });
  // We'll need this to undo step 2.
  Matrix4 rotateZInv({
    std::cos(-zAngle), -std::sin(-zAngle), 0, 0,
    std::sin(-zAngle),  std::cos(-zAngle), 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  });

  double actualAngle = rotateZ.dot(rotateX).dot(Vector4(r, 0)).x / 180 * M_PI;
  Matrix4 actualRotation({
    1, 0, 0, 0,
    0, std::cos(actualAngle), -std::sin(actualAngle), 0,
    0, std::sin(actualAngle),  std::cos(actualAngle), 0,
    0, 0, 0, 1
  });
  return rotateXInv.dot(rotateZInv).dot(actualRotation).dot(rotateZ).dot(rotateX);
}
Matrix4 translate(double x, double y, double z) {
  return Matrix4({
    1, 0, 0, x,
    0, 1, 0, y,
    0, 0, 1, z,
    0, 0, 0, 1
  });
}
