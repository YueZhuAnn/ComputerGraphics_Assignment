// Fall 2018

#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{}
};


// A v mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );
  virtual void applyTransform(glm::mat4 &trans, GeometryNode *parent) override;
  virtual bool intersect(Ray *ray, glm::vec3 &intersectPoint, glm::vec3 &normal, 
  						MaterialMap *map, GeometryNode *parent) override;
  virtual void calculateNormal(const glm::vec3 &eye) override;
  bool inBound(glm::vec3 &intersectP);
private:
	std::vector<glm::vec3> m_vertices;
	std::vector<glm::vec2> m_uvInfo;
	std::vector<glm::vec3> m_normals;
	std::vector<glm::vec3> m_VetexNormals;
	std::vector<Triangle> m_vNormals;
	std::vector<Triangle> m_vUVs;
	std::vector<Triangle> m_faces;
	double calculateArea(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
