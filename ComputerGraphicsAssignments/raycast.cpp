#include "raycast.h"
#define max(a, b) ((a) > (b) ? (a) : (b))

Vec3f get_depth_color(float t, float depth_min, float depth_max);
Vec3f get_diffuse_color(SceneParser& parser, Hit& hit, bool shade_back);
Vec3f get_normal_color(Hit& hit);

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
	char* normal_file = NULL;
	bool shade_back = false;
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
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
	//Main part
	Image out_image(width, height), depth_image(width, height), normal_image(width, height);
	SceneParser parser(input_file);
	Camera* camera = parser.getCamera();
	Group* group = parser.getGroup();
	float tmin = camera->getTMin();
	for (int i = 0; i < out_image.Width(); i++)
	{
		for (int j = 0; j < out_image.Height(); j++)
		{
			float x_bias = (i - out_image.Width() * 1.0 / 2) / out_image.Width();
			float y_bias = (j - out_image.Height() * 1.0 / 2) / out_image.Height();
			Ray ray = camera->generateRay(Vec2f(x_bias, y_bias));
			Hit hit;
			if (group->intersect(ray, hit, tmin))
			{
				//depth_file
				Vec3f depth_color = get_depth_color(hit.getT(), depth_min, depth_max);
				depth_image.SetPixel(i, j, depth_color);
				//diffuse_file(new out_file)			
				Vec3f diffuse_color = get_diffuse_color(parser, hit, shade_back);
				out_image.SetPixel(i, j, diffuse_color);
				//normal_file
				Vec3f normal_color = get_normal_color(hit);
				normal_image.SetPixel(i, j, normal_color);
			}
			else
			{
				out_image.SetPixel(i, j, parser.getBackgroundColor());
				depth_image.SetPixel(i, j, Vec3f(0, 0, 0));
				normal_image.SetPixel(i, j, Vec3f(0, 0, 0));
			}

		}
	}
	if (output_file)
		out_image.SaveTGA(output_file);
	if (depth_file)
		depth_image.SaveTGA(depth_file);
	if (normal_file)
		normal_image.SaveTGA(normal_file);
}

Vec3f get_depth_color(float t, float depth_min, float depth_max)
{
	if (t < depth_min)
		t = depth_min;
	if (t > depth_max)
		t = depth_max;
	float color = (1 - (t - depth_min) / (depth_max - depth_min));
	return Vec3f(color, color, color);
}

Vec3f get_diffuse_color(SceneParser& parser, Hit& hit, bool shade_back)
{
	Vec3f ambient = parser.getAmbientLight(), light_sum;
	light_sum = ambient;
	for (int k = 0; k < parser.getNumLights(); k++)
	{
		Light* light = parser.getLight(k);
		Vec3f p, dir, col;
		float tmp = 1.0;
		light->getIllumination(p, dir, col, tmp);
		if (shade_back)
			light_sum = light_sum + abs(dir.Dot3(hit.getNormal())) * col;
		else
			light_sum = light_sum + max(0, dir.Dot3(hit.getNormal())) * col;
	}
	return light_sum * hit.getMaterial()->getDiffuseColor();
}

Vec3f get_normal_color(Hit& hit)
{
	Vec3f normal = hit.getNormal();
	return Vec3f(abs(normal.x()), abs(normal.y()), abs(normal.z()));
}
