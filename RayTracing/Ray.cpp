#include <glm/ext.hpp>
#include <random>
#include "GeometryNode.hpp"
#include "Light.hpp"
#include "Primitive.hpp"
#include "MaterialMap.hpp"

using namespace std;
using namespace glm;

default_random_engine generator;
uniform_int_distribution<int>  distribution(0, 63);

bool Ray::intersect(const SceneNode * root, vec3 &pos, vec3 &normal, 
					MaterialMap*map, bool bShadowRay)
{
	if (!root) return false;
	for(auto it = root->children.begin(); it != root->children.end(); ++it)
	{
		NodeType type = (*it)->m_nodeType;
		if(type != NodeType::GeometryNode)
		{
			continue;
		}
		GeometryNode *geometryNode = static_cast<GeometryNode *>(*it);
		string test = geometryNode->m_name;
		vec3 intersectPoint;
		vec3 pointNormal;
		if(geometryNode->intersect(this, intersectPoint, pointNormal, map))
		{
			if(geometryNode->m_name == "sky")
			{
				pointNormal *= -1;
			}
			if(rayL == 0)
			{
				pos = intersectPoint;
				normal = pointNormal;
				node = geometryNode;
				rayL = length(pos-rO);
				if(rayL < rayLimit && !node->m_bRefraction) return true;
				continue;
			}
			bool update = false;
			for(uint i = 0; i < 3; i++)
			{
				if(rD[i] == 0) continue;
				if((rD[i] < 0 && intersectPoint[i] > pos[i]) ||
				(rD[i] > 0 && intersectPoint[i] < pos[i]))
				{
					update = true;
					break;
				}
			}
			if(update)
			{
				pos = intersectPoint;
				normal = pointNormal;
				node = geometryNode;
				rayL = length(pos-rO);
				if(rayL < rayLimit && !node->m_bRefraction) return true;
			}
		}
	}
	for(auto it = root->children.begin(); it != root->children.end(); ++it)
	{
		intersect((*it), pos, normal, map);
	}
	return node != NULL;
}

bool Ray::refractOut(const vec3 refractin, const vec3 normal, vec3 &refractout,
					const double inFactor, const double outFactor)
{
	if(inFactor > outFactor)
	{
		refractout = -refractin;
		return true;
	}
	vec3 interNormal = -normal;
	vec3 rotationAxis = cross(normal, refractin);
	double cosin = dot(normal, refractin);
	clamp(cosin, (double)-1.0, (double)1.0);
	if(cosin < 0)
	{
		cosin = abs(cosin);
		interNormal = -interNormal;
	}
	double ratio = inFactor/outFactor;
	double sinin = sqrtf(1-pow(cosin, 2));
	double sinout = sinin*inFactor/outFactor;
	if(sinout > 1)
	{
		return false;
	} 
	double inAngle = asin(sinin);
	double outAngle = asin(sinout);
	float rotateAngle = M_PI+inAngle-outAngle;
	mat4 refractRotate = rotate(mat4(), rotateAngle, rotationAxis);
	vec4 refractout4 = refractRotate*vec4(refractin, 0);
	refractout = {refractout4[0], refractout4[1],refractout4[2]};
	refractout /= length(refractout);
	return true;
	/*
	vec3 refractNormal = normal;
	double dotProduct = dot(refractin, normal);
	double ratio = inFactor/outFactor;
	if(dotProduct < 0)
	{
		refractNormal *= -1;
		dotProduct *= -1;
	}
	refractout = (-ratio*dotProduct -
				sqrtf(1-ratio*inFactor*pow(1-dotProduct,2)))*
				refractNormal+ratio*refractin;
	refractout /= length(refractout);
	return true;
	*/
}

bool Ray::reflectOut(const vec3 reflectin, const vec3 normal, vec3 &reflectout)
{
	 reflectout = 2*dot(normal, reflectin)*normal-reflectin; 
	 reflectout /= length(reflectout);
	 return true;
}

vec3 Ray::calculateReflection(const vec3 point, const vec3 rD, const vec3 normal, const SceneNode * root, 
								MaterialMap* map, bool &succeed, const vec3 ambient, const list<Light *> & lights)
{
	vec3 reflectRO = point;
	vec3 reflectin = -rD;
	vec3 reflectout;
	reflectOut(reflectin, normal, reflectout);
	Ray reflectRay{reflectRO, reflectout};
	vec3 reflectp = reflectRO;
	vec3 reflectn;
	succeed = reflectRay.intersect(root, reflectp, reflectn, map);
	if(succeed) return reflectRay.shader(reflectp, reflectn, root, ambient, lights, map);
	return vec3{0,0,0};
}

vec3 Ray::calculateRefraction(const vec3 point, const vec3 rD, const vec3 normal, double factor, const SceneNode * root, 
								MaterialMap* map, const vec3 ambient, const std::list<Light *> & lights)
{
	GeometryNode *innode = NULL;
	GeometryNode *outnode = node;
	double inFactor = 1;
	double outFactor = factor;
	vec3 refractRO = point;
	vec3 refractin = -rD;
	vec3 interfaceNormal = normal;
	vec3 refractout;
	if(!refractOut(refractin, interfaceNormal, refractout, inFactor, outFactor))
	{
		refractout = -refractin;
	}
	Color refractColor = {0,0,0};
	Ray refractRay{refractRO+0.0001*refractout, refractout};
	vec3 refractp = refractRO;
	vec3 refractn;
	string test;
	refractRay.intersect(root, refractp, refractn,map);
	if (refractRay.node == NULL) 
	{	
		return vec3{0,0,0};
	}
	while(refractRay.node->m_bRefraction)
	{
		// update refractRay
		innode = outnode;
		outnode = refractRay.node;
		inFactor = outFactor;
		outFactor = innode == outnode? 1: outnode->m_refractionFactor;
		refractRO = refractp;
		refractin = -refractRay.rD;
		interfaceNormal = refractn;
		if(!refractOut(refractin, interfaceNormal, refractout, inFactor, outFactor))
		{
			refractout = -refractin;
		}
		refractp = refractRO;
		refractRay = Ray{refractRO+0.0001*refractout, refractout}; 
		refractRay.rayL = 0;
		refractRay.intersect(root, refractp, refractn, map);
		if(refractRay.node == NULL)
		{ 
			return vec3{0,0,0};
		}
	}
	return refractRay.shader(refractp, refractn, root ,ambient, lights, map);
}


vec3 Ray::calculateShadowRay(vec3 p, vec3 n,  const SceneNode * root, PhongMaterial * material, 
								Light * light, MaterialMap *map)
{
	// calculate shadows
	bool bSky = false;
	string sName = node->m_name;
	if(sName == "sky") bSky = true;
	double totalEnergy = 0;
	vec3 xAxis{light->energy/8,0,0};
	vec3 zAxis{0,0,light->energy/8};
	vec3 lighto = light->position-4*xAxis-4*zAxis;
	vec3 pcolor{0,0,0};
	unsigned total = 36;
	vector<int> storedlight(64, 0);
	while(total > 0)
	{
		int lightindex = distribution(generator);
		if(storedlight[lightindex] != 0) continue;
		storedlight[lightindex] += 1;
		total--;
		vec3 mylightpos = lighto + (lightindex/8)*xAxis+(lightindex%8)*zAxis;
		vec3 myd = p-mylightpos;
		myd /= length(myd);
		mylightpos += 0.001*myd;
		vec3 intern;
		vec3 interp = mylightpos;
		Ray myray{mylightpos, myd};
		myray.rayLimit = length(mylightpos-p);
		bool bIntersect = myray.intersect(root, interp, intern, map, true);
		if(bSky)
		{
			pcolor += light->PhongModel(p,n, *material, node->m_bRefraction || node->m_bReflection, bSky);
			continue;
		}
		if(!bIntersect) 
		{
			pcolor += light->PhongModel(p,n,*material, node->m_bRefraction || node->m_bReflection, bSky);
			continue;
		}
		if(length(interp-mylightpos) > (length(p-mylightpos)-1))
		{
			pcolor += light->PhongModel(p,n, *material, node->m_bRefraction || node->m_bReflection, bSky);
			continue;
		}
		if(myray.node->m_bRefraction)
		{
			if(myray.node == node)
			{
				pcolor += light->PhongModel(p,n,*material, node->m_bRefraction || node->m_bReflection, bSky);
			}
			else if(node->m_bRefraction)
			{
				pcolor += 0.8*light->PhongModel(p,n,*material, node->m_bRefraction || node->m_bReflection, bSky);
			}
			else
			{
				// Cast shadow
				pcolor += 0.7*light->PhongModel(p,n,*material, node->m_bRefraction || node->m_bReflection, bSky);
			}
			continue;
		}
	}
	pcolor /= 36;
	if(node->m_bRefraction || node->m_bReflection){
		pcolor += 0.8*light->PhongModel(p,n, *material, true, bSky);
	}
	else
	{
		pcolor += 0.5*light->PhongModel(p,n, *material, false, bSky);
	}
	return pcolor;
}

vec3 Ray::shader(vec3 point, vec3 normal, const SceneNode * root, const glm::vec3 ambient,
				const list<Light *> & lights, MaterialMap *materialmap)
{
	bool bTexture = node->m_bTexture;
	Color origin = color;
	color = {0,0,0};
	Material *nodeMaterial = materialmap->getMaterial(node->m_materialID);
	PhongMaterial * oMaterial = static_cast<PhongMaterial *>(nodeMaterial);
	PhongMaterial * material = oMaterial;
	material = new PhongMaterial(oMaterial->m_kd, oMaterial->m_ks, oMaterial->m_shininess);
	if(bTexture && texture) 
	{
		material->m_kd = origin;
		material->m_ks = texture->getks();
		material->m_shininess = texture->getshininess();
	}
	if(node->m_bRefraction)
	{
		double factor = node-> m_refractionFactor;
		double transparency = node->m_transparency;
		vec3 refractColor = calculateRefraction(point, rD, normal, factor,
							root, materialmap, ambient, lights);
		refractColor = refractColor*ambient+refractColor;
		material->m_kd = transparency*material->m_kd + (1-transparency)*refractColor;
	}
	if(node->m_bReflection)
	{
		bool bSucceed;
		double factor = node -> m_reflectionFactor;
		vec3 reflectColor = factor*material->m_kd+calculateReflection(point, rD, normal, 
											root, materialmap, bSucceed, ambient, lights);
		reflectColor = reflectColor*ambient+reflectColor;
		if(bSucceed)
		{
			material->m_kd = factor*material->m_kd+(1-factor)*reflectColor;
		}
	}
	for(auto light = lights.begin(); light != lights.end(); ++light)
	{
		color += calculateShadowRay(point, normal, root, material,  *light, materialmap);
	}	
	delete material;
	return color;
}
