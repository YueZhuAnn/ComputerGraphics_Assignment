#include <vector>
#include <iosfwd>
#include <string>
#include <iostream>

#include <glm/ext.hpp>
#include <lodepng/lodepng.h>
#include "Texture.hpp"
#include "Image.hpp"

using namespace std;

Texture::Texture( const std::string& fname, glm::vec3 ks, double shininess):m_ks(ks), m_shininess(shininess)
{
	string sFilePath = "./Texture/"+fname;
	std::vector<unsigned char> mypic;
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

unsigned Texture::getWidth()
{
	return width;
}


unsigned Texture::getHeight()
{
	return height;
}

glm::vec3 Texture::getks()
{
	return m_ks;
}

double Texture::getshininess()
{
	return m_shininess;
}
glm::vec3 Texture::getColor(glm::vec2 p)
{
	uint indexW = (uint)(width*p[0]);
	uint indexH = (uint)(height*p[1]);
	if (indexW >= width) indexW = width-1;
	if (indexH >= height) indexH = height-1;
	return pic[indexH*width+indexW];
}

glm::vec3 Texture::getColor(std::vector<std::pair<glm::vec2, double>> weightInfo)
{
	glm::vec3 color;
	for (auto it = weightInfo.begin(); it != weightInfo.end(); ++it)
	{
		uint w = (uint)it->first[0];
		uint h = (uint)it->first[1];
		color += pic[h*width+w]*it->second;
	}
	return color;
}