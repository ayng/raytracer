#ifndef MATRIX_H
#define MATRIX_H

#include <initializer_list>
#include "Vector.h"
#include "Ray.h"

class Matrix4 {
  public:
    double v[16]; // Row-major.
    Matrix4 ();
    Matrix4 (const Matrix4& orig);
    Matrix4 (std::initializer_list<double> list);
    Matrix4 dot (const Matrix4& rhs) const;
    Vector4 dot (const Vector4& rhs) const;
    Ray transform (const Ray& rhs) const;
    void dump () const;
};

Matrix4 scale(double x, double y, double z);
Matrix4 rotate(double x, double y, double z);
Matrix4 translate(double x, double y, double z);

#endif
