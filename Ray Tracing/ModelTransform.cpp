#include <glm/ext.hpp>
#include <stack>
#include "ModelTransform.hpp"
#include "JointNode.hpp"
#include "GeometryNode.hpp"

using namespace glm;
using namespace std;

const vec3 xAxis = {1, 0, 0};
const vec3 yAxis = {0, 1, 0};
stack<mat4> transformStack;

void ModelTransform::calculateNormal(SceneNode *node, const glm::vec3 &eye)
{
	if (!node) return;
	if(node->m_nodeType == NodeType::GeometryNode)
	{
		GeometryNode * geometryNode = static_cast<GeometryNode *>(node);
		geometryNode->calculateNormal(eye);
	} 

	for (SceneNode * childNode : node->children) {
		calculateNormal(childNode, eye);
	}
}

void ModelTransform::applyTransform(SceneNode *node)
{
	if (!node) return;
	if(node->m_nodeType == NodeType::GeometryNode)
	{
		GeometryNode * geometryNode = static_cast<GeometryNode *>(node);
		geometryNode->applyTransform();
	} 

	for (SceneNode * childNode : node->children) {
		applyTransform(childNode);
	}
}

void ModelTransform::transformModel(SceneNode *node, SceneNode *parent)
{
	if (!node) return;
	NodeType type = node->m_nodeType;
	NodeType parentType = parent->m_nodeType;
	// set the transform mat	
	const mat4 prevTransformMatrix = node->get_transform();
	switch(parentType)
	{
		case NodeType::GeometryNode:
			transformStack.push(transformStack.top()*prevTransformMatrix);
			break;
		case NodeType::JointNode:
		{
			JointNode * jointNode = static_cast<JointNode *>(parent);
			JointNode::JointRange jointX = jointNode->m_joint_x;
			JointNode::JointRange jointY = jointNode->m_joint_y;
			mat4 rotX_matrix = rotate(mat4(), (float)jointX.init*(float)M_PI, xAxis);
			mat4 rotY_matrix = rotate(mat4(), (float)jointY.init*(float)M_PI, yAxis);
			transformStack.push(transformStack.top()*rotX_matrix*rotY_matrix*prevTransformMatrix);
			break;
		}
		default:
			// Initialize Stack
			if(transformStack.empty()){
				// Initialize Stack
				transformStack.push(prevTransformMatrix);
			}
			else
			{
				transformStack.push(transformStack.top()*prevTransformMatrix);
			}
	}
	node->set_transform(transformStack.top());
	for (SceneNode * childNode : node->children) {
		transformModel(childNode, node);
	}
	transformStack.pop();
}