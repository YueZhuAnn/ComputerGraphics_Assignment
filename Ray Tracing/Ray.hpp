#include<list>
#include <glm/glm.hpp>
#include "MaterialMap.hpp"
typedef glm::vec3 Color;

struct GeometryNode;
struct SceneNode;
struct Light;
struct Texture;
struct PhongMaterial;

struct Ray
{
	glm::vec3 rO;
	glm::vec3 rD;
	double rayL = 0;
	double rayLimit = 0;
	double refractionF = 1;
	GeometryNode *node = NULL;
	Texture *texture = NULL;
	bool bShader = false;
	Color color = {0,0,0};
	Ray(glm::vec3 rO, glm::vec3 rD, glm::vec3 color = Color{0,0,0}):rO(rO), rD(rD), color(color){};
	bool intersect(const SceneNode * root, glm::vec3 &pos, glm::vec3 &normal, MaterialMap* map, bool bShadowRay = false);
	glm::vec3 shader(glm::vec3 point, glm::vec3 normal, const SceneNode * root, const glm::vec3 ambient,
						const std::list<Light *> & lights, MaterialMap *materialmap);
private:	
	glm::vec3 calculateShadowRay(glm::vec3 p, glm::vec3 n,  const SceneNode * root, 
										PhongMaterial * material, Light * light, MaterialMap *map);
	glm::vec3 calculateReflection(const glm::vec3 point, const glm::vec3 rD, const glm::vec3 normal, const SceneNode * root, 
								MaterialMap* map, bool &succeed, const glm::vec3 ambient,const std::list<Light *> & lights);
	glm::vec3 calculateRefraction(const glm::vec3 point, const glm::vec3 rD, const glm::vec3 normal, double factor, const SceneNode * root, 
								MaterialMap* map, const glm::vec3 ambient, const std::list<Light *> & lights);
	bool refractOut(const glm::vec3 refractin, const glm::vec3 normal, glm::vec3 &refractout, const double inFactor, const double outFactor);
	bool reflectOut(const glm::vec3 reflectin, const glm::vec3 normal, glm::vec3 &reflectout);
};	