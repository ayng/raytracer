#ifndef CAMERA_H
#define CAMERA_H

#include "Vector.h"
#include "Matrix.h"

class Camera {
  public:
    Vector3 e, bl, br, tl, tr;
    Matrix4 transform;

    Camera ();
    Camera (const Vector3 &_e, const Vector3 &_bl, const Vector3 &_br,
            const Vector3 &_tl, const Vector3 &_tr);
};
#endif
