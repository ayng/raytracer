#include "Camera.h"

Camera::Camera () {}

void Camera::set (const Vector3& _e,
                  const Vector3& _bl, const Vector3& _br,
                  const Vector3& _tl, const Vector3& _tr) {
  e.copy(_e);
  bl.copy(_bl);
  br.copy(_br);
  tl.copy(_tl);
  tr.copy(_tr);
}
