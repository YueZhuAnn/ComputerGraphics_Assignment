// Fall 2018

#include <glm/ext.hpp>
#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim)
	: SceneNode( name )
	, m_primitive( prim )
	, m_materialID(0)
	, m_textureID(0)
	, m_bumpID(0)
{
	m_nodeType = NodeType::GeometryNode;
}

// deep copy
GeometryNode::GeometryNode(const GeometryNode & other)
	: SceneNode( (SceneNode)other )
	,m_bTexture( other.m_bTexture )
	,m_bReflection( other.m_bReflection )
	,m_reflectionFactor( other.m_reflectionFactor )
	,m_bRefraction( other.m_bRefraction )
	,m_refractionFactor( other.m_refractionFactor )
	,m_transparency( other.m_transparency )
	,m_materialID( other.m_materialID )
	,m_textureID( other.m_textureID )
	,m_bumpID( other.m_bumpID )
	,m_primitive ( other.m_primitive )
{}

void GeometryNode::setMaterial( unsigned id )
{
	if(id == m_materialID) return;
	m_materialID = id;
}

void GeometryNode::setTexture( unsigned id )
{
	if(id == m_textureID) return;
	m_bTexture = true;
	if (id == 0) m_bTexture = false;
	m_textureID = id;
}

void GeometryNode::setBumpMap( unsigned id )
{
	if(id == m_bumpID) return;
	m_bBumpmap = true;
	if(id == 0) m_bBumpmap = false;
	m_bumpID = id;
}

void GeometryNode::setReflection( double factor )
{
	m_bReflection = true;
	m_reflectionFactor = factor;
}

void GeometryNode::setRefraction( double factor, double transparency )
{
	m_bRefraction = true;
	m_refractionFactor = factor;
	m_transparency = transparency;
}

void GeometryNode::applyTransform()
{
	glm::mat4 trans = get_transform();
	m_primitive->applyTransform(trans, this);
}

void GeometryNode::calculateNormal(const glm::vec3 &eye)
{
	m_primitive->calculateNormal(eye);
}


bool GeometryNode::intersect(Ray *ray, glm::vec3 &intersectPoint, glm::vec3 &normal, MaterialMap *map)
{
	return m_primitive->intersect(ray, intersectPoint, normal, map, this);
}