#include "Matrix.h"

int main() {
  {
    Matrix4 s = scale(2, 3, 4);
    Matrix4 sInv = scale(1.0/2, 1.0/3, 1.0/4);
    Vector4 v(1, 1, 1, 1);
    s.dot(sInv).dot(v).dump();
  }
  {
    Matrix4 r = rotate(45, 0, 0);
    Vector4 v(1, 1, 1, 1);
    r.dot(v).dump();
  }

}
