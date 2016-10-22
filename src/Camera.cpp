#include "Camera.h"

Camera::Camera () {}

Camera::Camera (const Vector3& _e, const Vector3& _bl, const Vector3& _br,
  const Vector3& _tl, const Vector3& _tr)
  : e(_e), bl(_bl), br(_br), tl(_tl), tr(_tr) {}
