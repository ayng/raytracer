#ifndef VECTOR_H
#define VECTOR_H

class Vector3 {
  public:
    double x, y, z;
    Vector3 ();
    Vector3 (double _x, double _y, double _z);
    Vector3 (const Vector3& orig);
    void copy (const Vector3& orig);
    Vector3 operator + (const Vector3& rhs) const;
    Vector3 operator - (const Vector3& rhs) const;
    Vector3 operator * (const double rhs) const;
    bool operator == (const Vector3& rhs) const;
    double dot(const Vector3& rhs) const;
    Vector3 cross(const Vector3& rhs) const;
    double magnitude() const;
    Vector3 normalized() const;
    void dump();
};

Vector3 operator * (const double lhs, const Vector3& rhs);

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

#endif
