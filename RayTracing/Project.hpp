// Fall 2018

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include "GeometryNode.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "MaterialMap.hpp"
#include "Focal.hpp"

struct renderInfo
{
	uint y1;
	uint y2;
	uint x1;
	uint x2;
	uint k = 1;
	renderInfo(uint y1, uint y2, uint x1, uint x2):
		y1(y1), y2(y2), x1(x1), x2(x2){}
};

void Project_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights,

		// Focal len
		Focal *focal,
		
		// Material Map
		MaterialMap *materialmap
);


class Render
{
	// What to render
	SceneNode * root;

	// Image to write to, set to a given width and height
	Image &image;
	size_t h;
	size_t w;

	// Viewing parameters
	const glm::vec3 eye;
	const glm::vec3 view;
	const glm::vec3 scene;
	const glm::vec3 up;
	const glm::vec3 right;
	double fovy;

	// Lighting parameters
	const glm::vec3 ambient;
	const std::list<Light *> lights;
	uint  totalLight;

	// Background information
	unsigned bgwidth, bgheight;
	std::vector<unsigned char> background;

	// Image Buffer
	std::vector<std::vector<glm::vec3>> imageBuffer;

	// Super Sample
	uint pixel;
	unsigned threadCount;

	// Focal Len
	Focal *focal;

	// Material Map
	MaterialMap * materialmap;

	void renderImageThread(renderInfo *info);
	void asyncThread(const unsigned total);
public:
	Render(SceneNode * root, Image &image, size_t h, size_t w,
		glm::vec3 eye, glm::vec3 view, glm::vec3 scene, glm::vec3 up, 
		glm::vec3 right, double fovy, glm::vec3 ambient,
		const std::list<Light *> lights, Focal *focal, MaterialMap* map, unsigned threadCount, uint pixel=2):
		root(root), image(image), h(h), w(w), eye(eye), view(view),
		scene(scene), up(up), right(right), fovy(fovy), ambient(ambient),
		lights(lights), focal(focal), materialmap(map), threadCount(threadCount), pixel(pixel)
		{
			imageBuffer.resize(pixel*w+1);
			for(auto it = imageBuffer.begin(); it != imageBuffer.end(); ++it)
			{
				it->resize(pixel*h+1);
			}
			totalLight = lights.size();
		};

	void LoadBackgroundImage(std::string filename);

	void renderImage();
};
