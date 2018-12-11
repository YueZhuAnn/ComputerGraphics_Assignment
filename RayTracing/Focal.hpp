#pragma once

#include <iosfwd>

#include <glm/glm.hpp>

struct Focal {
  Focal(double p, double f, double n):P(p), F(f), n(n){};
  
  const double P;
  const double F;
  const double n;
};
