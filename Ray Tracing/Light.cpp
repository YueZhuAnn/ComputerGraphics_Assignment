// Fall 2018

#include <iostream>

#include <glm/ext.hpp>

#include "Light.hpp"

using namespace glm;
using namespace std;

Light::Light()
  : colour(0.0, 0.0, 0.0),
    position(0.0, 0.0, 0.0),
    energy(0)
{
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}

vec3 Light::PhongModel(vec3 &point, vec3 &normal, PhongMaterial &material, 
                bool bRefraction, bool bSky)
{
  // Direction from fragment to light source.
  vec3 l = normalize(position - point);
  // Direction from fragment to viewer (origin - fragpos).
  vec3 v = normalize(-point);

  float n_dot_l = fmax(dot(normal, l), brightness);
  if(bRefraction)
  {
    n_dot_l = fmax(dot(normal, l), 0.5);
  }
  float r = length(position-point);

	vec3 diffuse = material.m_kd * n_dot_l;
  vec3 specular = vec3(0.0);
  
  if (n_dot_l > 0.0) 
  {
    // Halfway vector.
    vec3 h = normalize(v + l);
    float n_dot_h = fmax(dot(normal, h), 0.0);
    specular = material.m_ks * pow(n_dot_h, material.m_shininess);
  }
  float attenuation = falloff[0]+r*falloff[1]+r*r*falloff[2];
  if(bSky)
  {
    attenuation *= 0.8;
    return diffuse *this->colour/attenuation;
  }
  return (diffuse + specular)*this->colour/attenuation;
}

ostream& operator<<(ostream& out, const Light& l)
{
  out << "L[" << to_string(l.colour) 
  	  << ", " << to_string(l.position) << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}
