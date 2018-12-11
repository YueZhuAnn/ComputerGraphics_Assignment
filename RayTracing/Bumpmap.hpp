#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

class Bumpmap {
public:
  	Bumpmap( const std::string& fname);
	glm::vec3 getNormal(glm::vec2 p);
	unsigned getWidth();
	unsigned getHeight();
private:
	std::vector<glm::vec3> pic;
	unsigned width, height;
	double xPrecision, yPrecision;
};
