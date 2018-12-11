#include <vector>
#include <iosfwd>
#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <lodepng/lodepng.h>
#include "Bumpmap.hpp"
#include "Image.hpp"

using namespace std;
using namespace glm;

const vec3 rotationAxis = vec3{1,0,0};
const mat4 rotateMatrix = rotate(mat4(), -(float)(0.5*M_PI), rotationAxis);
Bumpmap::Bumpmap( const string& fname)
{
	string sFilePath = "./Bump/"+fname;
	vector<unsigned char> mypic;
	unsigned error = lodepng::decode(mypic, width, height, sFilePath);
  	// If there's an error, display it.
  	if(error != 0)
  	{
    	cout << "Can't load texture image" << endl;
    	return;
  	}
	for(uint i = 0; i < height; i++)
	{
		for(uint k = 0; k < width; k++)
		{
			uint index = i*width+k;
			pic.push_back(glm::vec3{(float)mypic[index*4]/255,
				(float)mypic[index*4+1]/255,
				(float)mypic[index*4+2]/255});
		}
	}
	xPrecision = 1/width;
	yPrecision = 1/height;
}

unsigned Bumpmap::getWidth()
{
	return width;
}


unsigned Bumpmap::getHeight()
{
	return height;
}

vec3 Bumpmap::getNormal(glm::vec2 p)
{
	uint indexW = (uint)(width*p[0]);
	uint indexH = (uint)(height*p[1]);
	vec3 normal = normalize(pic[indexH*width+indexW]-vec3{0.5,0.5,0.5});
	vec4 normal_v4 = rotateMatrix*(vec4(normal, 0));
	return normalize(vec3{normal_v4[0], normal_v4[1], normal_v4[2]});
}
