#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

class Texture {
public:
  Texture( const std::string& fname, glm::vec3 kd, double shininess);
	glm::vec3 getColor(glm::vec2 p);
	glm::vec3 getColor(std::vector<std::pair<glm::vec2, double>> weightInfo);
	unsigned getWidth();
	unsigned getHeight();
	glm::vec3 getks();
	double getshininess();
private:
  	glm::vec3 m_ks;
	double m_shininess;
	std::vector<glm::vec3> pic;
	unsigned width, height;
	double xPrecision, yPrecision;
};
