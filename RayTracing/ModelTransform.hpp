#pragma once

#include <glm/glm.hpp>

class SceneNode;

class ModelTransform
{
    public:
    void calculateNormal(SceneNode *node, const glm::vec3 &eye);
    void applyTransform(SceneNode *node);
    void transformModel(SceneNode *node, SceneNode *parent);
};