#ifndef MATERIAL_H
#define MATERIAL_H

#include "Color.h"

struct Material {
  Color ka, kd, ks;
  double sp;
  Color kr;
};

#endif
