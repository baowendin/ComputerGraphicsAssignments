#include "raycast.h"
#include "image.h"
#include "scene_parser.h"
#include "orthographic.h"
#include "camera.h"
#include<iostream>
#include<string.h>
#include<assert.h>
void work(Image& image, Image& image1, SceneParser& parser, char* output_file, char* depth_file, float depth_min, float depth_max);

void ray_cast(int argc, char** argv)
{
	// ========================================================
// ========================================================
// Some sample code you might like to use for parsing 
// command line arguments 

	char* input_file = NULL;
	int width = 100;
	int height = 100;
	char* output_file = NULL;
	float depth_min = 0;
	float depth_max = 1;
	char* depth_file = NULL;

	// sample command line:
	// raytracer -input scene1_1.txt -size 200 200 -output output1_1.tga -depth 9 10 depth1_1.tga

	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-input")) {
			i++; assert(i < argc);
			input_file = argv[i];
		}
		else if (!strcmp(argv[i], "-size")) {
			i++; assert(i < argc);
			width = atoi(argv[i]);
			i++; assert(i < argc);
			height = atoi(argv[i]);
		}
		else if (!strcmp(argv[i], "-output")) {
			i++; assert(i < argc);
			output_file = argv[i];
		}
		else if (!strcmp(argv[i], "-depth")) {
			i++; assert(i < argc);
			depth_min = atof(argv[i]);
			i++; assert(i < argc);
			depth_max = atof(argv[i]);
			i++; assert(i < argc);
			depth_file = argv[i];
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
	Image image(width, height), image1(width, height);
	SceneParser parser(input_file);
	work(image, image1, parser, output_file, depth_file, depth_min, depth_max);
	// ========================================================
	// ========================================================
}

void work(Image& image, Image& image1, SceneParser& parser, char* output_file, char* depth_file, float depth_min, float depth_max)
{
	Camera* camera = parser.getCamera();
	Group* group = parser.getGroup();
	float tmin = camera->getTMin();
	for (int i = 0; i < image.Width(); i++)
	{
		for (int j = 0; j < image.Height(); j++)
		{
			float x_bias = (i - image.Width() * 1.0 / 2) / image.Width();
			float y_bias = (j - image.Height() * 1.0 / 2) / image.Height();
			Ray ray = camera->generateRay(Vec2f(x_bias, y_bias));
			Hit hit;
			if (group->intersect(ray, hit, tmin))
			{
				image.SetPixel(i, j, hit.getMaterial()->getDiffuseColor());
				float t = hit.getT();
				if (t < depth_min)
					t = depth_min;
				if (t > depth_max)
					t = depth_max;
				float color = (1 - (t - depth_min) / (depth_max - depth_min));
				image1.SetPixel(i, j, Vec3f(color , color, color));
				//cout << color << endl;
			}
			else
			{
				image.SetPixel(i, j, parser.getBackgroundColor());
				image1.SetPixel(i, j, Vec3f(0, 0, 0));
			}
				
		}
	}
	image.SaveTGA(output_file);
	image1.SaveTGA(depth_file);
}