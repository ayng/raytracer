#ifndef VECTOR_H
#define VECTOR_H

#include <cmath>

class Vector3 {
  public:
    double x, y, z;
    Vector3 ();
    Vector3 (double _x, double _y, double _z);
    Vector3 (const Vector3& orig);
    Vector3 operator + (const Vector3& rhs) const;
    Vector3 operator - (const Vector3& rhs) const;
    Vector3 operator * (const double rhs) const;
    bool operator == (const Vector3& rhs) const;
    bool isDefined() const;
    double dot(const Vector3& rhs) const;
    Vector3 cross(const Vector3& rhs) const;
    double magnitude() const;
    Vector3 normalized() const;
    void dump() const;
};

Vector3 operator * (const double lhs, const Vector3& rhs);

const Vector3 NAN_VECTOR (nan(""), nan(""), nan(""));

class Vector2 {
  public:
    double x, y;
    Vector2 ();
    Vector2 (const double _x, const double _y);
    Vector2 (const Vector2& orig);
    Vector2 operator + (const Vector2& rhs) const;
    Vector2 operator - (const Vector2& rhs) const;
    double magnitude () const;
};

class Vector4 {
  public:
    double x, y, z, w;
    Vector4 ();
    Vector4 (double _x, double _y, double _z, double _w);
    Vector4 (const Vector3& v, double _w);
    Vector4 (const Vector4& orig);
    double dot (const Vector4& rhs) const;
    Vector3 toVector3 () const;
    void dump() const;
};

#endif
