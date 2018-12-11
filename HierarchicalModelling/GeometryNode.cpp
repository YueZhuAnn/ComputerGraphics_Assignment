// Fall 2018

#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
		const std::string & meshId,
		const std::string & name,
		bool able
)
	: SceneNode(name),
	  meshId(meshId),
	  ableSelect(able)
{
	unsigned b = m_nodeId >> 16;
	unsigned g = (m_nodeId-(b << 16)) >> 8;
	unsigned r = m_nodeId - (b << 16) - (g << 8);
	material.kf = glm::vec3((float)r/255.0, (float)g/255.0, (float)b/255.0);
	m_nodeType = NodeType::GeometryNode;
}
