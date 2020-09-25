#include "rayTrace.h"
// we set them as global parameter so they can be accessed by trace()
char* input_file = NULL;
int width = 100;
int height = 100;
char* output_file = NULL;
float depth_min = 0;
float depth_max = 1;
char* depth_file = NULL;
char* normal_file = NULL;
bool shade_back = false, gui_used = false, gouraud_used = false;
bool shadows = false;
int max_bounces = 1, cutoff_weight;
int theta_num = 20, phi_num = 10;
SceneParser* parser;
void shade();
void trace(float x, float y);
void ray_trace(int argc, char** argv)
{
	// ========================================================
	// ========================================================
	// Some sample code you might like to use for parsing 
	// command line arguments 

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
		else if (!strcmp(argv[i], "-normals")) {
			i++; assert(i < argc);
			normal_file = argv[i];
		}
		else if (!strcmp(argv[i], "-depth")) {
			i++; assert(i < argc);
			depth_min = atof(argv[i]);
			i++; assert(i < argc);
			depth_max = atof(argv[i]);
			i++; assert(i < argc);
			depth_file = argv[i];
		}
		else if (!strcmp(argv[i], "-shade_back"))
		{
			shade_back = true;
		}
		else if (!strcmp(argv[i], "-gui")) {
			gui_used = true;
		}
		else if (!strcmp(argv[i], "-tessellation")) {
			i++; assert(i < argc);
			theta_num = (int)atof(argv[i]);
			i++; assert(i < argc);
			phi_num = (int)atof(argv[i]);

		}
		else if (!strcmp(argv[i], "-gouraud")) {
			gouraud_used = true;
		}
		else if (!strcmp(argv[i], "-shadows")) {
			shadows = true;
		}
		else if (!strcmp(argv[i], "-bounces")) {
			i++; assert(i < argc);
			max_bounces = (int)atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-weight")) {
			i++; assert(i < argc);
			cutoff_weight = atof(argv[i]);
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
	parser = new SceneParser(input_file);
	//Main Code Part
	if (gui_used)
	{
		GLCanvas glcanvas;
		glcanvas.initialize(parser, shade, trace);
	}
	else
	{
		shade();
	}
}

void shade()
{
	Image out_image(width, height);
	Camera* camera = parser->getCamera();
	float tmin = camera->getTMin();
	RayTracer raytracer(parser, max_bounces, cutoff_weight, shadows);
	for (int i = 0; i < out_image.Width(); i++)
	{
		for (int j = 0; j < out_image.Height(); j++)
		{
			float x_bias = (i - out_image.Width() * 1.0 / 2) / out_image.Width();
			float y_bias = (j - out_image.Height() * 1.0 / 2) / out_image.Height();
			Ray ray = camera->generateRay(Vec2f(x_bias, y_bias));
			Hit hit;
			Vec3f color = raytracer.traceRay(ray, tmin, 0, 0, 1, hit);
			out_image.SetPixel(i, j, color);
		}

	}
	if (output_file)
		out_image.SaveTGA(output_file);
}

void trace(float x, float y)
{
	Camera* camera = parser->getCamera();
	float tmin = camera->getTMin();
	RayTracer raytracer(parser, max_bounces, cutoff_weight, shadows);
	Vec2f point(x - 0.5, y - 0.5);
	Ray ray = camera->generateRay(point);
	Hit hit_result;
	Vec3f Color = raytracer.traceRay(ray, camera->getTMin(), 0, cutoff_weight, 0, hit_result);
}