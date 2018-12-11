// Fall 2018

#pragma once

#include <iosfwd>

#include <glm/glm.hpp>
#include "PhongMaterial.hpp"


struct GeometryNode;

struct Light {
  Light();
  
  glm::vec3 colour;
  glm::vec3 position;
  double falloff[3];
  double energy;
  double brightness;
  glm::vec3 PhongModel(glm::vec3 &point, glm::vec3 &normal, PhongMaterial &material, 
                      bool bRefraction = false, bool bSky = true);
};

std::ostream& operator<<(std::ostream& out, const Light& l);
