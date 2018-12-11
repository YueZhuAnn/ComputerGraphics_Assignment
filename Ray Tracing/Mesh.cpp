// Fall 2018

#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#include "GeometryNode.hpp"

using namespace std;
using namespace glm;

const vec3 yAxis{0,1,0};

void indexEncode(const string &info, size_t &v, size_t &n, size_t &uv)
{
	unsigned size = info.length();
	// initialize
	v = n = uv = 0;
	uint start = 0;
	uint end = 0;
	uint index = 0;
	for(uint i = 0; i < size; i++)
	{
		if(info[i] == '/')
		{
			end = i;
			if(end-start == 0)
			{
				index++;
				start = end+1;
				continue;
			}
			index++;
			switch(index)
			{
				case 1:
				{
					v = stoi(info.substr(start, end-start));
					break;
				}
				case 2:
				{
					uv = stoi(info.substr(start, end-start));
					break;
				}
			}
			start = end+1;
		}
	}
	index++;
	end = size;
	if(start < end)
	{
		switch(index)
		{
			case 1:
			{
				v = stoi(info.substr(start, end-start));
				break;
			}
			case 2:
			{
				uv = stoi(info.substr(start, end-start));
				break;
			}
			default:
			{
				n = stoi(info.substr(start, end-start));
				break;
			}
		}
	}
}

Mesh::Mesh( const string& fname )
	: m_vertices()
	, m_faces()
{
	string code;
	string sInput;
	double vx, vy, vz;
	double nx, ny, nz;
	double tx, ty;
	string s1, s2, s3;

	string name = "./Model/"+fname;
	ifstream ifs( name.c_str() );
	while(getline(ifs, sInput))
	{
		istringstream iss (sInput);
		iss >> code;
		if( code == "v" ) {
			iss >> vx >> vy >> vz;
			m_vertices.push_back( vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			iss >> s1 >> s2 >> s3;
			size_t v1, v2, v3;
			size_t n1, n2, n3;
			size_t uv1, uv2, uv3;
			indexEncode(s1, v1, n1, uv1);
			indexEncode(s2, v2, n2, uv2);
			indexEncode(s3, v3, n3, uv3);
			m_faces.push_back( Triangle( v1 - 1, v2 - 1, v3 - 1 ) );
			if(n1 != 0)
			{
				m_vNormals.push_back( Triangle( n1-1, n2-1, n3-1) );
			}
			if(uv1 != 0)
			{
				m_vUVs.push_back( Triangle( uv1-1, uv2-1, uv3-1) );
			}
		} else if( code == "vn" ){
			iss >> nx >> ny >> nz;
			m_VetexNormals.push_back( vec3 (nx, ny, nz) );
		} else if( code == "vt")
		{
			iss >> tx >> ty;
			m_uvInfo.push_back( vec2( tx, ty) );
		}
	}
}

double Mesh::calculateArea(vec3 p1, vec3 p2, vec3 p3)
{
	return length(cross(p2-p1, p3-p1));
}


bool sameSide(vec3 edge, vec3 p1, vec3 p2)
{
	return dot(cross(edge, p1), cross(edge, p2)) >= 0;
}

void Mesh::calculateNormal(const vec3 &eye)
{
	if(m_normals.size() != 0) return;
	unsigned size = m_faces.size();
	for(unsigned i = 0; i < size; i++)
	{
		Triangle face = m_faces[i];
		vec3 p1 = m_vertices[face.v1];
		vec3 p2 = m_vertices[face.v2];
		vec3 p3 = m_vertices[face.v3];
		vec3 middle = (p1+p2+p3)/3;
		vec3 v1 = p3-p1;
		vec3 v2 = p2-p1;
		vec3 normal = cross(v1, v2);
		if(dot(eye-middle, normal) < 0)
		{
			normal *= -1;
		}
		normal /= length(normal);
		m_normals.push_back(normal);
	}
}

void Mesh::applyTransform(mat4 &trans, GeometryNode *parent)
{
	m_trans.push_back(trans);
	m_inverseTrans.push_back(inverse(trans));
	parent->m_pID = m_inverseTrans.size()-1;
	if(m_applyTransform) return;
    m_applyTransform = true;
	bool init = false;
	double xmin, xmax, ymin, ymax, zmin, zmax = 0;
	for(auto it = m_vertices.begin(); it != m_vertices.end(); ++it)
	{
		vec4 newVertice = vec4{*it, 1};
		*it = vec3{newVertice[0], newVertice[1], newVertice[2]};
		double x = newVertice[0];
		double y = newVertice[1];
		double z = newVertice[2];
		if(!init)
		{
			xmin = xmax = x;
			ymin = ymax = y;
			zmin = zmax = z;
			init = true;
		}
		else
		{
			if (x < xmin) xmin = x;
			if (x > xmax) xmax = x;
			if (y < ymin) ymin = y;
			if (y > ymax) ymax = y;
			if (z < zmin) zmin = z;
			if (z > zmax) zmax = z;
		}
	}
	const double bound = 0.000001;
	xmin -= bound;
	ymin -= bound;
	zmin -= bound;
	xmax += bound;
	ymax += bound;
	zmax += bound;
	m_limit.push_back({xmin, ymin, zmin});
	m_limit.push_back({xmax, ymax, zmax});
}

bool Mesh::inBound(vec3 &intersectP)
{
	for(uint i = 0; i < 3; i++)
	{
		double test = (double)intersectP[i];
		if(m_limit[0][i] <= test && test <= m_limit[1][i]) continue;
		return false;
	}
	return true;
}

bool Mesh::intersect(Ray *ray, vec3 &intersectPoint, vec3 &normal, 
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
    float t = 0;
	const double testBound = 0.001;
    Ray *testray  = new Ray{rO, rD, ray->color};
    if (!checkBound(testray, intersectPoint, normal))
    {
        delete testray;
        return false;
    }
    delete testray;
	if(BV)
    {
        vec4 transPoint = m_trans[pID] * vec4(intersectPoint, 1);
	    intersectPoint = {transPoint[0],transPoint[1],transPoint[2]};
        vec4 rotateNormal = m_inverseTrans[pID]*vec4(normal, 0);
        normal = {rotateNormal[0],rotateNormal[1],rotateNormal[2]};
        normal /= length(normal);
        return true;
    }
	bool bIntersect = false;
	const unsigned size = m_faces.size();
	for(unsigned i = 0; i < size; i++)
	{
		Triangle face = m_faces[i];
		vec3 p1 = m_vertices[face.v1];
		vec3 p2 = m_vertices[face.v2];
		vec3 p3 = m_vertices[face.v3];
		vec3 v1 = p3-p1;
		vec3 v2 = p2-p1;
		vec3 n  = m_normals[i];
		double d = dot(p1-rO, n);
		double t = d/dot(rD, n);
		if(t <= 0.00001) continue;
		if(isnan(t)) continue;
		vec3 intersectP = rO + t* rD;
		double mytest = length(intersectP-rO);
		if(length(intersectP-rO) < 0.00001) continue;
		double total = calculateArea(p1,p2,p3);
		double a1 = calculateArea(intersectP,p2,p3);
		double a2 = calculateArea(intersectP,p1,p3);
		double a3 = total-a1-a2;
		vector<vec3> points{p1,p2,p3};
		if(!sameSide(p2-p1, p3-p1, intersectP-p1) ||
		 !sameSide(p3-p2, p1-p2, intersectP-p2) ||
		 !sameSide(p1-p3, p2-p3, intersectP-p3)) continue;
 		if(!bIntersect)
		{
			bIntersect = true;
			if(m_texture)
			{
				Triangle UVface = m_vUVs[i];
				vec2 pUV = (a1*m_uvInfo[UVface.v1]+ 
				a2*m_uvInfo[UVface.v2]+ a3*m_uvInfo[UVface.v3])/total;
				textureC =  m_texture->getColor(pUV);
				bFillTexture = true;
			}
			intersectPoint = intersectP;
			vec3 v1n;
			vec3 v2n;
			vec3 v3n;
			if(m_VetexNormals.size() != 0)
			{
				Triangle Normalface = m_vNormals[i];
				v1n = m_VetexNormals[Normalface.v1];
				v2n = m_VetexNormals[Normalface.v2];
				v3n = m_VetexNormals[Normalface.v3];
			}
			else
			{
				v1n = v2n = v3n = m_normals[i];
			}
			normal = (a1*v1n+a2*v2n+a3*v3n)/total;
			if(m_bumpmap)
			{
				Triangle UVface = m_vUVs[i];
				vec2 pUV = (a1*m_uvInfo[UVface.v1]+ 
				a2*m_uvInfo[UVface.v2]+ a3*m_uvInfo[UVface.v3])/total;
				vec3 bnormal = m_bumpmap->getNormal(pUV);
				if(normal[1] == 1)
				{
					normal = bnormal;
				}
				else
				{
					mat3 rotM;
					rotM[0] = normal;
					rotM[1] = normalize(cross(rotM[0], yAxis));
					rotM[2] = normalize(cross(rotM[0], rotM[1]));
					normal = rotM*bnormal;
				}
			}
			continue;
		}
		if(length(intersectP-rO) < length(intersectPoint-rO))
		{
			if(m_texture)
			{
				Triangle UVface = m_vUVs[i];
				vec2 pUV = (a1*m_uvInfo[UVface.v1]+ 
				a2*m_uvInfo[UVface.v2]+ a3*m_uvInfo[UVface.v3])/total;
				textureC =  m_texture->getColor(pUV);
				bFillTexture = true;
			}
			intersectPoint = intersectP;
			vec3 v1n;
			vec3 v2n;
			vec3 v3n;
			if(m_VetexNormals.size() != 0)
			{
				Triangle Normalface = m_vNormals[i];
				v1n = m_VetexNormals[Normalface.v1];
				v2n = m_VetexNormals[Normalface.v2];
				v3n = m_VetexNormals[Normalface.v3];
			}
			else
			{
				v1n = v2n = v3n = m_normals[i];
			}
			normal = (a1*v1n+a2*v2n+a3*v3n)/total;
			if(m_bumpmap)
			{
				Triangle UVface = m_vUVs[i];
				vec2 pUV = (a1*m_uvInfo[UVface.v1]+ 
				a2*m_uvInfo[UVface.v2]+ a3*m_uvInfo[UVface.v3])/total;
				vec3 bnormal = m_bumpmap->getNormal(pUV);
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
		}
	}
	if(bIntersect)
	{
		vec4 transPoint = m_trans[pID] * vec4(intersectPoint, 1);
		vec4 rotateNormal = transpose(rotateinverse)*vec4(normal, 0);
		intersectPoint = {transPoint[0],transPoint[1],transPoint[2]};
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

ostream& operator<<(ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}
