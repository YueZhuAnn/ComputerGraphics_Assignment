// Fall 2018

#include <iostream>
#include <glm/ext.hpp>
#include "Primitive.hpp"
#include "polyroots.hpp"
#include "GeometryNode.hpp"
#include "Texture.hpp"
#include "Bumpmap.hpp"
#include "PolyCal.hpp"

using namespace std;
using namespace glm;

const vec3 yAxis{0,1,0};

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

Cylinder::~Cylinder()
{
}

Cone::~Cone()
{
}

Torus::~Torus()
{
}

bool Primitive::intersect(Ray *ray, vec3 &intersectPoint, vec3 &normal, 
                        MaterialMap *map, GeometryNode *parent)
{
    return false;
}

void Primitive::applyTransform(mat4 &trans, GeometryNode *parent){}

void Primitive::calculateNormal(const vec3 &eye){}

bool Primitive::checkBound(Ray *ray, vec3 &intersectPoint, vec3 &normal){ 
    vec3 rO = ray->rO;
    vec3 rD = ray->rD;
    float t = 0;
    float testBound = 0.001;
    for(unsigned i = 0; i < 3; i++)
    {
        double mytest1 = m_limit[0][i];
        double mytest2 = m_limit[1][i];
        float mytest3 = rO[i];
        float testT1 = (m_limit[0][i]-rO[i])/rD[i];
        float testT2 = (m_limit[1][i]-rO[i])/rD[i];
        bool bIntersect = true;
        if(testT1 > testBound)
        {
            for(unsigned k = 0; k < 3; k++)
            {
                if(k == i) continue;
                double newPoint = rO[k]+testT1*rD[k];
                if(m_limit[0][k] <= newPoint && newPoint <= m_limit[1][k])
                {
                    continue;
                }
                bIntersect = false;
                break;
            }
            if(bIntersect && (t == 0 || testT1 < t))
            {
                t = testT1;
                if(BV){
                    intersectPoint = rO + testT1*rD;
                    normal = {0,0,0};
                    normal[i] = -1;
                }
                else
                {
                    return true;
                }
            }
        }
        if(testT2 > testBound){
            bIntersect = true;
            for(unsigned k = 0; k < 3; k++)
            {
                if(k == i) continue;
                double newPoint = rO[k]+testT2*rD[k];
                if(m_limit[0][k] <= newPoint && newPoint <= m_limit[1][k])
                {
                    continue;
                }
                bIntersect = false;
                break;
            }
            if(bIntersect && (t == 0 || testT2 < t))
            {
                t = testT2;
                if(BV)
                {
                    intersectPoint = rO + testT2*rD;
                    normal = {0,0,0};
                    normal[i] = 1;
                }
                else
                {
                    return true;
                }
            }
        }
    }
    if(t != 0)
    {
        return true;
    }
    return false;
}

void Sphere::applyTransform(mat4 &trans, GeometryNode *parent)
{
    m_trans.push_back(trans);
    m_inverseTrans.push_back(inverse(trans));
    parent->m_pID = m_inverseTrans.size()-1;
    if(m_applyTransform) return;
    m_applyTransform = true;
    m_axisSize.push_back(m_radius);
    m_axisSize.push_back(m_radius);
    m_axisSize.push_back(m_radius);
    m_limit.push_back({m_pos[0]-m_axisSize[0], m_pos[1]-m_axisSize[1], m_pos[2]-m_axisSize[2]});
    m_limit.push_back({m_pos[0]+m_axisSize[0], m_pos[1]+m_axisSize[1], m_pos[2]+m_axisSize[2]});
}

void Cube::applyTransform(mat4 &trans, GeometryNode *parent)
{
    m_trans.push_back(trans);
    m_inverseTrans.push_back(inverse(trans));
    parent->m_pID = m_inverseTrans.size()-1;
    if(m_applyTransform) return;
    m_applyTransform = true;
    m_axisSize.push_back(m_size);
    m_axisSize.push_back(m_size);
    m_axisSize.push_back(m_size);
    m_limit.push_back({m_pos[0], m_pos[1], m_pos[2]});
    m_limit.push_back({m_pos[0]+m_axisSize[0], m_pos[1]+m_axisSize[1], m_pos[2]+m_axisSize[2]});
}

void Cylinder::applyTransform(mat4 &trans, GeometryNode *parent)
{
    m_trans.push_back(trans);
    m_inverseTrans.push_back(inverse(trans));
    parent->m_pID = m_inverseTrans.size()-1;
    if(m_applyTransform) return;
    m_applyTransform = true;
    double m_size = m_radius;
    m_axisSize.push_back(m_size);
    m_axisSize.push_back(m_height);
    m_axisSize.push_back(m_size);
    m_limit.push_back({m_pos[0]-m_axisSize[0], m_pos[1], m_pos[2]-m_axisSize[1]});
    m_limit.push_back({m_pos[0]+m_axisSize[0], m_pos[1]+m_axisSize[1], m_pos[2]+m_axisSize[2]});
}

void Cone::applyTransform(mat4 &trans, GeometryNode *parent)
{
    m_trans.push_back(trans);
    m_inverseTrans.push_back(inverse(trans));
    parent->m_pID = m_inverseTrans.size()-1;
    if(m_applyTransform) return;
    m_applyTransform = true;
    double m_size = m_radius;
    m_axisSize.push_back(m_size);
    m_axisSize.push_back(m_height);
    m_axisSize.push_back(m_size);
    m_limit.push_back({m_pos[0]-m_axisSize[0], m_pos[1], m_pos[2]-m_axisSize[1]});
    m_limit.push_back({m_pos[0]+m_axisSize[0], m_pos[1]+m_axisSize[1], m_pos[2]+m_axisSize[2]});
}

void Torus::applyTransform(mat4 &trans, GeometryNode *parent)
{
    m_trans.push_back(trans);
    m_inverseTrans.push_back(inverse(trans));
    parent->m_pID = m_inverseTrans.size()-1;
    if(m_applyTransform) return;
    m_applyTransform = true;
    double m_size = m_R;
    m_axisSize.push_back(m_size);
    m_axisSize.push_back(m_size);
    m_axisSize.push_back(m_size);
    m_limit.push_back({m_pos[0]-m_axisSize[0], m_pos[1]-m_axisSize[0], m_pos[2]-m_axisSize[1]});
    m_limit.push_back({m_pos[0]+m_axisSize[0], m_pos[1]+m_axisSize[1], m_pos[2]+m_axisSize[2]});
}

bool Sphere::intersect(Ray *ray, vec3 &intersectPoint, vec3 &normal, 
                    MaterialMap *map, GeometryNode *parent)
{
    unsigned pID = parent->m_pID;
    Texture* m_texture = map->getTexture(parent->m_textureID);
    Bumpmap* m_bumpmap = map->getBumpmap(parent->m_bumpID);
    vec3 textureC;
	bool bFillTexture = false;
    vec3 rO = ray->rO;
    vec3 rD = ray->rD;
    mat4 rotateinverse = m_inverseTrans[pID];
    vec4 inverseRO = m_inverseTrans[pID]*vec4{rO, 1};
    vec4 inverseRD = rotateinverse*vec4{rD, 0};
    rO = {inverseRO[0], inverseRO[1], inverseRO[2]};
    rD = {inverseRD[0], inverseRD[1], inverseRD[2]};
    rD /= length(rD);
    Ray *testray  = new Ray{rO, rD, ray->color};
    if (!checkBound(testray, intersectPoint, normal))
    {
        delete testray;
        return false;
    }
    delete testray;
    double t = 0;
    double roots[2];
    double a, b, c = 0;
    double d1 = pow(m_axisSize[0], 2);
    double d2 = pow(m_axisSize[1], 2);
    double d3 = pow(m_axisSize[2], 2);
    a = pow(rD[0], 2)*d2*d3+pow(rD[1], 2)*d1*d3+pow(rD[2], 2)*d1*d2;
    b = 2 *((rO[0]-m_pos[0])*rD[0]*d2*d3+(rO[1]-m_pos[1])*rD[1]*d1*d3+(rO[2]-m_pos[2])*rD[2]*d1*d2);
    c = pow(rO[0]-m_pos[0], 2)*d2*d3+ pow(rO[1]-m_pos[1], 2)*d1*d3 + pow(rO[2]-m_pos[2], 2)*d1*d2 - d1*d2*d3;
    quadraticRoots(a, b, c, roots);

    if (roots[0] > 0) t = roots[0];

    if (roots[1] > 0 && (t == 0 || roots[1] < t)) t = roots[1];
    if (t > 0.1 || (ray->bShader && t > 0.007))
    {
        intersectPoint = rO+t*rD;
        double dz = (intersectPoint[2]-m_pos[2])/m_axisSize[2];
        double dy = (intersectPoint[1]-m_pos[1])/m_axisSize[1];
        double dx = (intersectPoint[0]-m_pos[0])/m_axisSize[0];
        float u = 0.5+atan2(dz, dy)/(2*M_PI);
        float v = 0.5-asin(dx)/M_PI;
        if(m_texture)
        {
            textureC =  m_texture->getColor(vec2{u,v});
            bFillTexture = true;
        }
        for(unsigned i = 0; i < 3; i++)
        {
            normal[i] = intersectPoint[i]-m_pos[i];
            normal[i] /= pow(m_axisSize[i], 2);
        }
        if(m_bumpmap)
		{
            vec3 bnormal = m_bumpmap->getNormal(vec2{u,v});
            if(0.99 <= normal[1] && normal[1] <= 1.01)
			{
				normal = bnormal;
			}
			else
			{
				vec3 rotateAxis = normalize(cross(yAxis, normal));
				double costheta = -dot(normal, yAxis);
				double sintheta = sqrtf(1-pow(costheta, 2));
				normal = bnormal*costheta+cross(rotateAxis, bnormal)*sintheta+
				rotateAxis*dot(rotateAxis, bnormal)*(1-costheta);
			}
        }
        vec4 transPoint = m_trans[pID] * vec4(intersectPoint, 1);
		intersectPoint = {transPoint[0],transPoint[1],transPoint[2]};
        vec4 rotateNormal = transpose(rotateinverse)*vec4(normal, 0);
        if(((length(intersectPoint-ray->rO) < ray->rayL) || (0 == ray->rayL)) && bFillTexture)
		{
			ray->color = textureC;
			ray->texture = m_texture;
		}
        normal = {rotateNormal[0],rotateNormal[1],rotateNormal[2]};
        normal /= length(normal);
        return true;
    }
    return false;
}

bool Cube::intersect(Ray *ray, vec3 &intersectPoint, vec3 &normal, 
                    MaterialMap *map, GeometryNode *parent)
{
    unsigned pID = parent->m_pID;
    Texture* m_texture = map->getTexture(parent->m_textureID);
    Bumpmap* m_bumpmap = map->getBumpmap(parent->m_bumpID);
    vec3 textureC;
	bool bFillTexture = false;
    vec3 rO = ray->rO;
    vec3 rD = ray->rD;
    mat4 rotateinverse = m_inverseTrans[pID];
    vec4 inverseRO = m_inverseTrans[pID]*vec4{rO, 1};
    vec4 inverseRD = rotateinverse*vec4{rD, 0};
    rO = {inverseRO[0], inverseRO[1], inverseRO[2]};
    rD = {inverseRD[0], inverseRD[1], inverseRD[2]};
    float testBound = 0.001;
    float t = 0;
    Ray *testray  = new Ray{rO, rD, ray->color};
    if (!checkBound(testray, intersectPoint, normal))
    {
        delete testray;
        return false;
    }
    delete testray;
    uint interFace = 0;
    for(unsigned i = 0; i < 3; i++)
    {
        float testT1 = (m_pos[i]-rO[i])/rD[i];
        float testT2 = (m_pos[i]+m_axisSize[i]-rO[i])/rD[i];
        bool bIntersect = true;
        if(testT1 > testBound && length(testT1*rD) >= 0.1)
        {
            for(unsigned k = 0; k < 3; k++)
            {
                if(k == i) continue;
                double newPoint = rO[k]+testT1*rD[k];
                double test1 = m_pos[k];
                double test2 = m_axisSize[k];
                if(m_pos[k] <= newPoint && newPoint <= m_pos[k]+m_axisSize[k])
                {
                    continue;
                }
                bIntersect = false;
                break;
            }
            if(bIntersect && (t == 0 || testT1 < t))
            {
                t = testT1;
                intersectPoint = rO + testT1*rD;
                normal = {0,0,0};
                normal[i] = -1;
                interFace = i;
            }
        }
        if(testT2 > testBound && length(testT2*rD) >= 0.1){
            bIntersect = true;
            for(unsigned k = 0; k < 3; k++)
            {
                if(k == i) continue;
                double newPoint = rO[k]+testT2*rD[k];
                double test1 = m_pos[k];
                double test2 = m_axisSize[k];
                if(m_pos[k] <= newPoint && newPoint <= m_pos[k]+m_axisSize[k])
                {
                    continue;
                }
                bIntersect = false;
                break;
            }
            if(bIntersect && (t == 0 || testT2 < t))
            {
                t = testT2;
                intersectPoint = rO + testT2*rD;
                normal = {0,0,0};
                normal[i] = 1;
                interFace = i;
            }
        }
    }
    if(t != 0)
    {
        double u, v;
        switch(interFace)
        {
            case 0:
            {
                u = (intersectPoint[1]-m_pos[1])/(m_axisSize[1]);
                v = (intersectPoint[2]-m_pos[2])/(m_axisSize[2]);
                break;
            }
            case 1:
            {
                u = (intersectPoint[0]-m_pos[0])/(m_axisSize[0]);
                v = (intersectPoint[2]-m_pos[2])/(m_axisSize[2]);
                break;
            }
            default:
            {
                u = (intersectPoint[0]-m_pos[0])/(m_axisSize[0]);
                v = (intersectPoint[1]-m_pos[1])/(m_axisSize[1]);
                break;
            }
        }
        if(m_texture)
        {
            textureC =  m_texture->getColor(vec2{u,v});
            bFillTexture = true;
        }
        if(m_bumpmap)
		{
            vec3 bnormal = m_bumpmap->getNormal(vec2{u,v});
            if(0.99 <= normal[1] && normal[1] <= 1.01)
			{
				normal = bnormal;
			}
			else
			{
				vec3 rotateAxis = normalize(cross(yAxis, normal));
				double costheta = dot(normal, yAxis);
				double sintheta = sqrtf(1-pow(costheta, 2));
				normal = bnormal*costheta+cross(rotateAxis, bnormal)*sintheta+
				rotateAxis*dot(rotateAxis, bnormal)*(1-costheta);
			}
        }
        vec4 transPoint = m_trans[pID] * vec4(intersectPoint, 1);
		intersectPoint = {transPoint[0],transPoint[1],transPoint[2]};
        vec4 rotateNormal = transpose(rotateinverse)*vec4(normal, 0);
        if(((length(intersectPoint-ray->rO) < ray->rayL) || (0 == ray->rayL)) && bFillTexture)
		{
			ray->color = textureC;
			ray->texture = m_texture;
		}
        normal = {rotateNormal[0],rotateNormal[1],rotateNormal[2]};
		normal /= length(normal);
        return true;
    }
    return false;
}

bool Cylinder::intersect(Ray *ray, vec3 &intersectPoint, vec3 &normal,
                        MaterialMap *map, GeometryNode *parent)
{
    unsigned pID = parent->m_pID;
    Texture* m_texture = map->getTexture(parent->m_textureID);
    Bumpmap* m_bumpmap = map->getBumpmap(parent->m_bumpID);
    vec3 textureC;
	bool bFillTexture = false;
    vec3 rO = ray->rO;
    vec3 rD = ray->rD;
    mat4 rotateinverse = m_inverseTrans[pID];
    vec4 inverseRO = m_inverseTrans[pID]*vec4{rO, 1};
    vec4 inverseRD = rotateinverse*vec4{rD, 0};
    rO = {inverseRO[0], inverseRO[1], inverseRO[2]};
    rD = {inverseRD[0], inverseRD[1], inverseRD[2]};
    rD /= length(rD);

    Ray *testray  = new Ray{rO, rD};
    if (!checkBound(testray, intersectPoint, normal))
    {
        delete testray;
        return false;
    }
    delete testray;

    double t = 0;
    double roots[2];
    double a, b, c = 0;
    double d = pow(m_radius, 2);
    a = pow(rD[0], 2)+pow(rD[2], 2);
    b = 2*(rO[0]*rD[0]-m_pos[0]*rD[0]+rO[2]*rD[2]-m_pos[2]*rD[2]);
    c = pow(rO[0],2)-2*rO[0]*m_pos[0]+pow(m_pos[0],2)
        +pow(rO[2],2)-2*rO[2]*m_pos[2]+pow(m_pos[2],2)
        - d;

    quadraticRoots(a, b, c, roots);
    if (roots[0] > 0)
    {
        vec3 testP = rO+roots[0]*rD;
        if(m_pos[1] < testP[1] && testP[1] < m_pos[1]+m_height)
        {
            t = roots[0];
        }
    }
    if (roots[1] > 0 && (t == 0 || roots[1] < t))
    {
        vec3 testP = rO+roots[1]*rD;
        if(m_pos[1] < testP[1] && testP[1] < m_pos[1]+m_height)
        {
            t = roots[1];
        }
    }
    // Check top and bottom
    double test1  = (m_pos[1]-rO[1])/rD[1];
    double test2  = (m_pos[1]+m_height-rO[1])/rD[1];
    if(test1 > 0)
    {
        vec3 testP = rO+test1*rD;
        if(pow(testP[0]-m_pos[0],2)+pow(testP[2]-m_pos[2],2) <= pow(m_radius, 2)+0.01)
        {
            if(t == 0 || test1 < t) t = test1;
        }
    }
    if(test2 > 0)
    {
        vec3 testP = rO+test2*rD;
        if(pow(testP[0]-m_pos[0],2)+pow(testP[2]-m_pos[2],2) <= pow(m_radius, 2)+0.01)
        {
            if(t == 0 || test2 < t) t = test2;
        }
    }
    if (t > 0.1 || (ray->bShader && t > 0.007))
    {
        intersectPoint = rO+t*rD;
        double u,v;
        if(m_pos[1]-0.01 < intersectPoint[1] && m_pos[1]+0.01 > intersectPoint[1])
        {
            vec3 op = m_pos+vec3{-m_radius, 0, -m_radius};
            u = (intersectPoint[0]-op[0])/(2*m_radius);
            v = (intersectPoint[2]-op[2])/(2*m_radius);
            normal = vec3{0,-1,0};
            if(u > 1 || v > 1)
            {
                float test = 1;
            }
        }
        else if(m_pos[1]+m_height-0.01 < intersectPoint[1] && m_pos[1]+m_height+0.01 > intersectPoint[1])
        {
            vec3 op = m_pos+vec3{-m_radius, 0, -m_radius};
            u = (intersectPoint[0]-op[0])/(2*m_radius);
            v = (intersectPoint[2]-op[2])/(2*m_radius);
            normal = vec3{0,1,0};
            if(u > 1 || v > 1)
            {
                float test = 1;
            }
        }
        else
        {
            float mycos = dot(intersectPoint-m_pos-vec3{0,intersectPoint[1],0}, 
                            vec3{-1,0,0})/m_radius;
            float angle = acos(mycos);
            if((intersectPoint-m_pos)[2] < 0)
            {
                u = (2*M_PI-angle)/(2*M_PI);
            }
            else
            {
                u = angle/(2*M_PI);
            }
            v = (intersectPoint[1]-m_pos[1])/m_height;
            if(u > 1 || v > 1)
            {
                float test = 1;
            }
            normal = vec3{intersectPoint[0]-m_pos[0],0,intersectPoint[2]-m_pos[2]};
        }
        if(m_texture)
        {
            textureC =  m_texture->getColor(vec2{u,v});
            bFillTexture = true;
        }
        if(m_bumpmap)
		{
            vec3 bnormal = m_bumpmap->getNormal(vec2{u,v});
            if(0.99 <= normal[1] && normal[1] <= 1.01)
			{
				normal = bnormal;
			}
			else
			{
				vec3 rotateAxis = normalize(cross(yAxis, normal));
				double costheta = -dot(normal, yAxis);
				double sintheta = sqrtf(1-pow(costheta, 2));
				normal = bnormal*costheta+cross(rotateAxis, bnormal)*sintheta+
				rotateAxis*dot(rotateAxis, bnormal)*(1-costheta);
			}
        }
        vec4 transPoint = m_trans[pID] * vec4(intersectPoint, 1);
		intersectPoint = {transPoint[0],transPoint[1],transPoint[2]};
        vec4 rotateNormal = transpose(rotateinverse)*vec4(normal, 0);
        if(((length(intersectPoint-ray->rO) < ray->rayL) || (0 == ray->rayL)) && bFillTexture)
		{
			ray->color = textureC;
			ray->texture = m_texture;
		}
        normal = {rotateNormal[0],rotateNormal[1],rotateNormal[2]};
        normal /= length(normal);
        return true;
    }
    return false;
}

bool Cone::intersect(Ray *ray, vec3 &intersectPoint, vec3 &normal,
                        MaterialMap *map, GeometryNode *parent)
{
    unsigned pID = parent->m_pID;
    Texture* m_texture = map->getTexture(parent->m_textureID);
    Bumpmap* m_bumpmap = map->getBumpmap(parent->m_bumpID);
    vec3 textureC;
	bool bFillTexture = false;
    vec3 rO = ray->rO;
    vec3 rD = ray->rD;
    mat4 rotateinverse = m_inverseTrans[pID];
    vec4 inverseRO = m_inverseTrans[pID]*vec4{rO, 1};
    vec4 inverseRD = rotateinverse*vec4{rD, 0};
    rO = {inverseRO[0], inverseRO[1], inverseRO[2]};
    rD = {inverseRD[0], inverseRD[1], inverseRD[2]};
    rD /= length(rD);

    Ray *testray  = new Ray{rO, rD};
    if (!checkBound(testray, intersectPoint, normal))
    {
        delete testray;
        return false;
    }
    delete testray;

    double t = 0;
    double roots[2];
    double a, b, c = 0;
    double d = pow(m_radius/m_height, 2);
    a = pow(rD[0], 2)-pow(rD[1], 2)*d+pow(rD[2], 2);
    b = 2*(rO[0]*rD[0]-m_pos[0]*rD[0]-
            rO[1]*rD[1]*d+m_pos[1]*rD[1]*d
            +rO[2]*rD[2]-m_pos[2]*rD[2]);
    c = pow(rO[0],2)-2*rO[0]*m_pos[0]+pow(m_pos[0],2)
        -d*pow(rO[1],2)+2*rO[1]*m_pos[1]*d-d*pow(m_pos[1],2)
        +pow(rO[2],2)-2*rO[2]*m_pos[2]+pow(m_pos[2],2);

    quadraticRoots(a, b, c, roots);
    if (roots[0] > 0)
    {
        vec3 testP = rO+roots[0]*rD;
        if(m_pos[1] < testP[1] && testP[1] < m_pos[1]+m_height)
        {
            t = roots[0];
        }
    }
    if (roots[1] > 0 && (t == 0 || roots[1] < t))
    {
        vec3 testP = rO+roots[1]*rD;
        if(m_pos[1] < testP[1] && testP[1] < m_pos[1]+m_height)
        {
            t = roots[1];
        }
    }
    double test  = (m_pos[1]+m_height-rO[1])/rD[1];
    if(test > 0)
    {
        vec3 testP = rO+test*rD;
        if(pow(testP[0]-m_pos[0],2)+pow(testP[2]-m_pos[2],2) <= pow(m_radius, 2)+0.01)
        {
            if(t == 0 || test < t) t = test;
        }
    }
    if (t > 0.1 || (ray->bShader && t > 0.007))
    {
        intersectPoint = rO+t*rD;
        double u,v;
        if(m_pos[1]+m_height-0.01 < intersectPoint[1] && m_pos[1]+m_height+0.01 > intersectPoint[1])
        {
            normal = vec3{0,1,0};
        }
        else
        {
            normal = vec3{intersectPoint[0]-m_pos[0],0,intersectPoint[2]-m_pos[2]};
            normal[1] = -length(normal)*m_radius/m_height;
        }
        vec3 op = m_pos+vec3{-m_radius, 0, -m_radius};
        u = (intersectPoint[0]-op[0])/(2*m_radius);
        v = (intersectPoint[2]-op[2])/(2*m_radius);
        if(m_texture)
        {
            textureC =  m_texture->getColor(vec2{u,v});
            bFillTexture = true;
        }
        if(m_bumpmap)
		{
            vec3 bnormal = m_bumpmap->getNormal(vec2{u,v});
            if(0.99 <= normal[1] && normal[1] <= 1.01)
			{
				normal = bnormal;
			}
			else
			{
				vec3 rotateAxis = normalize(cross(yAxis, normal));
				double costheta = -dot(normal, yAxis);
				double sintheta = sqrtf(1-pow(costheta, 2));
				normal = bnormal*costheta+cross(rotateAxis, bnormal)*sintheta+
				rotateAxis*dot(rotateAxis, bnormal)*(1-costheta);
			}
        }
        vec4 transPoint = m_trans[pID] * vec4(intersectPoint, 1);
		intersectPoint = {transPoint[0],transPoint[1],transPoint[2]};
        vec4 rotateNormal = transpose(rotateinverse)*vec4(normal, 0);
        if(((length(intersectPoint-ray->rO) < ray->rayL) || (0 == ray->rayL)) && bFillTexture)
		{
			ray->color = textureC;
			ray->texture = m_texture;
		}
        normal = {rotateNormal[0],rotateNormal[1],rotateNormal[2]};
        normal /= length(normal);
        return true;
    }
    return false;
}

bool Torus::intersect(Ray *ray, vec3 &intersectPoint, vec3 &normal,
                        MaterialMap *map, GeometryNode *parent)
{
    unsigned pID = parent->m_pID;
    Texture* m_texture = map->getTexture(parent->m_textureID);
    Bumpmap* m_bumpmap = map->getBumpmap(parent->m_bumpID);
    vec3 textureC;
	bool bFillTexture = false;
    vec3 rO = ray->rO;
    vec3 rD = ray->rD;
    mat4 rotateinverse = m_inverseTrans[pID];
    vec4 inverseRO = m_inverseTrans[pID]*vec4{rO, 1};
    vec4 inverseRD = rotateinverse*vec4{rD, 0};
    rO = {inverseRO[0], inverseRO[1], inverseRO[2]};
    rD = {inverseRD[0], inverseRD[1], inverseRD[2]};
    rD /= length(rD);

    Ray *testray  = new Ray{rO, rD};
    if (!checkBound(testray, intersectPoint, normal))
    {
        delete testray;
        return false;
    }
    delete testray;
    double t = 0;
    double roots[4];
    double a,b,c,d,e=0;
    poly polyX{rD[0], rO[0]-m_pos[0]};
    poly polyY{rD[1], rO[1]-m_pos[1]};
    poly polyZ{rD[2], rO[2]-m_pos[2]};
    poly Rmr{pow(m_R,2)-pow(m_r,2)};
    poly xs = polyMult(polyX, polyX);
    poly ys = polyMult(polyY, polyY);
    poly zs = polyMult(polyZ, polyZ);
    poly result = polyAdd(xs, ys);
    result = polyAdd(result, zs);
    result = polyAdd(result, Rmr);
    result = polyMult(result, result);
    result = polyAdd(result, polyScale(polyAdd(xs, zs), -4*pow(m_R, 2)));
    a = result[0];
    b = result[1];
    c = result[2];
    d = result[3];
    e = result[4];
    quarticRoots(b/a,c/a,d/a,e/a, roots);

    if (roots[0] > 0.1 && roots[0] < 1000)
    {
        t = roots[0];
    }
    if (roots[1] > 0.1 && roots[1] < 1000 && (t == 0 || roots[1] < t))
    {
        t = roots[1];
    }
    if (roots[2] > 0.1 && roots[2] < 1000 && (t == 0 || roots[2] < t))
    {
        t = roots[2];
    }
    if (roots[3] > 0.1 && roots[3] < 1000 && (t == 0 || roots[3] < t))
    {
        t = roots[3];
    }
    if (t > 0.1 || (ray->bShader && t > 0.007))
    {
        intersectPoint = rO+t*rD;
        vec3 of = intersectPoint-m_pos;
        of -= vec3{0,of[1],0};
        vec3 op = m_pos+of*(m_R+m_r)/2/length(of);
        normal = intersectPoint-op;
        double u, v;
        op = m_pos+vec3{-m_R, 0, -m_R};
        u = (intersectPoint[0]-op[0])/(2*m_R);
        v = (intersectPoint[2]-op[2])/(2*m_R);
        if(m_texture)
        {
            textureC =  m_texture->getColor(vec2{u,v});
            bFillTexture = true;
        }
        if(m_bumpmap)
		{
            vec3 bnormal = m_bumpmap->getNormal(vec2{u,v});
            if(0.99 <= normal[1] && normal[1] <= 1.01)
			{
				normal = bnormal;
			}
			else
			{
				vec3 rotateAxis = normalize(cross(yAxis, normal));
				double costheta = -dot(normal, yAxis);
				double sintheta = sqrtf(1-pow(costheta, 2));
				normal = bnormal*costheta+cross(rotateAxis, bnormal)*sintheta+
				rotateAxis*dot(rotateAxis, bnormal)*(1-costheta);
			}
        }
        vec4 transPoint = m_trans[pID] * vec4(intersectPoint, 1);
		intersectPoint = {transPoint[0],transPoint[1],transPoint[2]};
        vec4 rotateNormal = transpose(rotateinverse)*vec4(normal, 0);
        if(((length(intersectPoint-ray->rO) < ray->rayL) || (0 == ray->rayL)) && bFillTexture)
		{
			ray->color = textureC;
			ray->texture = m_texture;
		}
        normal = {rotateNormal[0],rotateNormal[1],rotateNormal[2]};
        normal /= length(normal);
        return true;
    }
    return false;
}
