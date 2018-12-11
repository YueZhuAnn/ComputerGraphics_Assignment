// Fall 2018

#include <glm/ext.hpp>
#include <lodepng/lodepng.h>
#include <thread>
#include <random>
#include <future>
#include <mutex>
#include "Project.hpp"
#include "ModelTransform.hpp"
#include "MaterialMap.hpp"

using namespace std;
using namespace glm;

#define pixelCount 2

mutex mtx;           
vector<unsigned> process(200,0);
unsigned current = 0;
unsigned threadrow = 0;
unsigned threadcolumn = 0;

void Render::asyncThread(const unsigned total)
{
	renderInfo *info = NULL;
	unsigned index = 0;
	unsigned threadprocess = 0;
	uint startw, endw, starth, endh;
	while(current < total)
	{
		//delete info;
		mtx.lock();
			if (current >= total) 
			{
				mtx.unlock();
				break;
			}
			threadprocess = current*100/total;
			if(process[threadprocess] == 0)
			{
				cout << threadprocess << "%" << endl;
				process[threadprocess]++;
			}
			index = current;
			current++; 
		mtx.unlock();
		startw = index % threadrow;
		starth = index / threadrow;
		startw *= 5;
		endw = startw+5;
		if(index % threadrow == threadrow-1)
		{
			endw = pixel*w+1;
		}
		starth *= 5;
		endh = starth+5;
		if(index / threadrow == threadcolumn-1)
		{
			endh = pixel*h+1;
		}
		info = new renderInfo(starth, endh, startw, endw);
		renderImageThread(info);
	}
	delete info;
}

void Render::renderImageThread(renderInfo *info)
{
	uint y1 = info->y1;
	uint y2 = info->y2;
	uint x1 = info->x1;
	uint x2 = info->x2;
	for (int y = y1; y < y2; ++y) {
		for (int x = x1; x < x2; ++x) {
			int height = pixel*h/2-y;
			int width = x-pixel*w/2;
			vec3 rO = scene + height*up +  width*right;
			vec3 rD = rO-eye;
			rD /= length(rD);
			rO = eye;
			Ray ray{rO, rD, ambient};
			vec3 p = rO;
			vec3 normal;
			if(!ray.intersect(root, p, normal, materialmap))
			{
				uint bgW = x*bgwidth/w/pixel;
				uint bgH = y*bgheight/h/pixel;
				uint index = bgH*bgwidth+bgW;
				vec3 bgc{0,0,0};
				imageBuffer[x][y] = bgc;
			}
			else
			{
				Color pixelColor = ray.shader(p, normal, root, ambient, lights, materialmap);
				imageBuffer[x][y] =ambient*pixelColor+pixelColor;
				double d = abs(p[2]-eye[2]);
				double f = focal->F;
				double p = focal->P;
				double a = focal->n;
				double c = abs(pixel*a*f*(p-d)/d/(p-f));
				double i = abs(1/(1/f-1/p));
				double r = abs(c/2*(scene[2]-eye[2]));
				int iR = int(r);
				if(iR-r >= 0.5) iR += 1;
				if (iR > 6*pixel/5) iR = (int)6*pixel/5;
				if (iR <= 4*pixel/5) continue;
				// Find focal point
				double t = (p-eye[2])/rD[2];
				float total = 1.0;
				float totalLight = 1.0;
				vec3 focalp = eye+t*rD;
				vec3 oPixel = scene + height*up +  width*right;
				vec3 frO;
				vec3 frD;
				vec3 fp;
				vec3 fnormal;
				Color fpixelColor;
				for(int p = -iR; p <= iR; p++){
					for(int q = -iR; q <= iR; q++)
					{
						if(p == 0 && q == 0) continue;
						vec3 fPixel = oPixel;
						fPixel[0] += q;
						fPixel[1] += p;
						frD = normalize(focalp-fPixel);
						if(frD[2] > 0) frD *= -1;
						t = (focalp[2]-eye[2])/frD[2];
						frO = focalp-t*frD;
						Ray fray{frO, rD, ambient};
						fp = frO;
						fray.intersect(root, fp, fnormal, materialmap);
						fpixelColor = fray.shader(fp, fnormal, root, ambient, lights, materialmap);
						imageBuffer[x][y] +=ambient*fpixelColor+fpixelColor;
						total+=1.0;
					}
				}
				imageBuffer[x][y] /= total;
			}
		}
	}
}

void Render::renderImage()
{
	threadrow = (pixel*w)/5;
	threadcolumn = (pixel*h)/5;
	unsigned total = threadrow*threadcolumn;
	thread *renderThread = new thread[threadCount];
	for(uint i = 0; i < threadCount; i++)
	{
		renderThread[i] = thread(&Render::asyncThread,this, total);
	}
	for (int i = 0; i < threadCount; i++)
	{
		renderThread[i].join();
	}
	double p = focal->P;
	double a = focal->n;
	double f = abs(focal->F-eye[2]);
	double rMax = 10*a;

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x)
		{
			Color color = {0,0,0};
			uint totalV = (pixel)*(pixel);
			Color color1 = {0,0,0};
			Color color2 = {0,0,0};
			Color color3 = {0,0,0};
			for(uint p = 0; p <= pixel; p++){
				for(uint q = 0; q <= pixel; q++)
				{
					if((p == 0 || p == pixel) && (q == 0 || q == pixel))
					{
						color1 += imageBuffer[pixel*x+p][pixel*y+q];
						continue;
					}
					if(p == 0 || p == pixel || q == 0 || q == pixel)
					{
						color2 += imageBuffer[pixel*x+p][pixel*y+q];
						continue;
					}
					color3 += imageBuffer[pixel*x+p][pixel*y+q];
				}
			}
			color = color1 /(totalV*4)+color2/(totalV*2)+color3/totalV;
			image(x,y,0) = color[0];
			image(x,y,1) = color[1];
			image(x,y,2) = color[2];
		}
	}

	delete [] renderThread;
	unsigned id = 1;
	while(true)
	{
		if(NULL == materialmap->getTexture(id))
		{
			break;
		}
		delete materialmap->getTexture(id);
		id++;
	}
	id = 1;
	while(true)
	{
		if(NULL == materialmap->getMaterial(id))
		{
			break;
		}
		delete materialmap->getMaterial(id);
		id++;
	}
	id = 1;
	while(true)
	{
		if(NULL == materialmap->getBumpmap(id))
		{
			break;
		}
		delete materialmap->getBumpmap(id);
		id++;
	}
	cout << "Render Complete!" << endl;
}

void Project_Render(
		// What to render  
		SceneNode * root,

		// Image to write to, set to a given width and height  
		Image & image,

		// Viewing parameters  
		const vec3 & eye,
		const vec3 & view,
		const vec3 & up,
		double fovy,

		// Lighting parameters  
		const vec3 & ambient,
		const list<Light *> & lights,

		// Focal len
		Focal *focal,

		// Material Map
		MaterialMap *materialmap
) {

	ModelTransform transform;

	for(SceneNode * childNode : root->children)
	{
		transform.transformModel(childNode, root);
	}

	transform.applyTransform(root);

	transform.calculateNormal(root, eye);

	unsigned int n = thread::hardware_concurrency();
	cout << "Support threads: "<< n << endl;
	
	size_t h = image.height();
	size_t w = image.width();
	uint k = 1;
	
	vec3 scene = eye;
	scene += pixelCount*(float)image.height()/2/tan(fovy/2*M_PI/180)*view;
	vec4 right_4 = rotate(mat4(), (float)M_PI/2, view)*vec4{up, 0};
	vec3 right = vec3{right_4[0], right_4[1],right_4[2]};

	Render *render = new Render(root, image, h, w, eye, view, scene,
					up, right, fovy, ambient, lights, focal,
					materialmap, n, pixelCount);

	render->renderImage();

	delete render;
}
