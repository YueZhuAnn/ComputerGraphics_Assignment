// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Ray.hpp"
#include "Texture.hpp"
#include "Bumpmap.hpp"

const bool BV = false;

class Primitive {
public:
  virtual bool intersect(Ray *ray, glm::vec3 &intersectPoint, glm::vec3 &normal, 
                        MaterialMap *map, GeometryNode *parent);
  virtual void applyTransform(glm::mat4 &trans, GeometryNode *parent);
  virtual void calculateNormal(const glm::vec3 &eye);
  virtual bool checkBound(Ray *ray, glm::vec3 &intersectPoint, glm::vec3 &normal);
  virtual ~Primitive();
  bool m_applyTransform = false;
  std::vector<glm::mat4> m_trans;
  std::vector<glm::mat4> m_inverseTrans;
  std::vector<std::vector<double>> m_limit;
};

class Sphere : public Primitive {
public:
  Sphere(const glm::vec3& pos = glm::vec3{0,0,0}, double radius = 1):
      m_pos(pos), m_radius(radius){};
  virtual void applyTransform(glm::mat4 &trans, GeometryNode *parent) override;
  virtual bool intersect(Ray *ray, glm::vec3 &intersectPoint, glm::vec3 &normal, 
                        MaterialMap *map, GeometryNode *parent) override;
  virtual ~Sphere();
protected:
  glm::vec3 m_pos;
  double m_radius;
  std::vector<double> m_axisSize;
};

class Cube : public Primitive {
public:
  Cube(const glm::vec3& pos = glm::vec3{0,0,0}, double size = 1):
        m_pos(pos), m_size(size){};
  virtual bool intersect(Ray *ray, glm::vec3 &intersectPoint, glm::vec3 &normal, 
                        MaterialMap *map, GeometryNode *parent) override;
  virtual void applyTransform(glm::mat4 &trans, GeometryNode *parent) override;
  virtual ~Cube();
protected:
  glm::vec3 m_pos;
  double m_size;
  std::vector<double> m_axisSize;
};

class Cylinder : public Primitive {
public:
  Cylinder(const glm::vec3& pos, double height, double radius):
          m_pos(pos), m_height(height), m_radius(radius){};
  virtual bool intersect(Ray *ray, glm::vec3 &intersectPoint, glm::vec3 &normal, 
                        MaterialMap *map, GeometryNode *parent) override;
  virtual void applyTransform(glm::mat4 &trans, GeometryNode *parent) override;
  virtual ~Cylinder();
private:
  glm::vec3 m_pos;
  double m_height;
  double m_radius;
  std::vector<double> m_axisSize;
};

class Cone : public Primitive {
public:
  Cone(const glm::vec3& pos, double height, double radius):
          m_pos(pos), m_height(height), m_radius(radius){};
  virtual bool intersect(Ray *ray, glm::vec3 &intersectPoint, glm::vec3 &normal, 
                        MaterialMap *map, GeometryNode *parent) override;
  virtual void applyTransform(glm::mat4 &trans, GeometryNode *parent) override;
  virtual ~Cone();
private:
  glm::vec3 m_pos;
  double m_height;
  double m_radius;
  std::vector<double> m_axisSize;
};

class Torus : public Primitive {
public:
  Torus(const glm::vec3& pos, double R, double r):
          m_pos(pos), m_R(R), m_r(r){};
  virtual bool intersect(Ray *ray, glm::vec3 &intersectPoint, glm::vec3 &normal, 
                        MaterialMap *map, GeometryNode *parent) override;
  virtual void applyTransform(glm::mat4 &trans, GeometryNode *parent) override;
  virtual ~Torus();
private:
  glm::vec3 m_pos;
  double m_R;
  double m_r;
  std::vector<double> m_axisSize;
};
