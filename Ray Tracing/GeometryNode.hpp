// Fall 2018

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "Texture.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim);
	GeometryNode(const GeometryNode & other);
	void setMaterial( unsigned id );
	void setTexture( unsigned id );
	void setBumpMap( unsigned id );
	void setReflection( double factor );
	void setRefraction( double factor, double transparency );
	void applyTransform();
	void calculateNormal(const glm::vec3 &eye);
	bool intersect(Ray *ray, glm::vec3 &intersectPoint, glm::vec3 &normal, MaterialMap *map);
	unsigned int m_pID = 0;

	// For reflection
	bool m_bReflection = false;
	double m_reflectionFactor;
	
	// For refraction
	bool m_bRefraction = false;
	double m_refractionFactor;
	double m_transparency;

	// For material
	unsigned m_materialID;
	unsigned m_textureID;
	unsigned m_bumpID;
	bool m_bTexture = false;
	bool m_bBumpmap = false;
	
	// For primitive
	Primitive *m_primitive;
};
