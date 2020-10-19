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
char* sample_file = NULL;
char* filter_file = NULL;
bool shade_back = false, gui_used = false, gouraud_used = false, grid_used = false, visualize_grid = false, tracing_stats = false;
bool render_samples = false, render_filter = false;
int nx, ny, nz;
bool shadows = false;
int max_bounces = 0, cutoff_weight;
int theta_num = 20, phi_num = 10;
int num_samples = 1;
int sample_zoom;
int filter_zoom;
Sampler* sampler;
Filter* filter;
SceneParser* parser;
Grid* grid = NULL;
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
		else if (!strcmp(argv[i], "-grid")) {
			grid_used = true;
			i++; assert(i < argc);
			nx = atof(argv[i]);
			i++; assert(i < argc);
			ny = atof(argv[i]);
			i++; assert(i < argc);
			nz = atof(argv[i]);
		}
		else if (!strcmp(argv[i], "-visualize_grid")) {
			visualize_grid = true;
		}
		else if (!strcmp(argv[i], "-stats")) {
			tracing_stats = true;
		}
		else if (!strcmp(argv[i], "-render_samples")) {
			i++; assert(i < argc);
			sample_file = argv[i];
			i++; assert(i < argc);
			sample_zoom = atoi(argv[i]);
			render_samples = true;
		}
		else if (!strcmp(argv[i], "-random_samples")) {
			i++; assert(i < argc);
			num_samples = atoi(argv[i]);
			sampler = new RandomSampler(num_samples);
		}
		else if (!strcmp(argv[i], "-uniform_samples")) {
			i++; assert(i < argc);
			num_samples = atoi(argv[i]);
			sampler = new UniformSampler(num_samples);
		}
		else if (!strcmp(argv[i], "-jittered_samples")) {
			i++; assert(i < argc);
			num_samples = atoi(argv[i]);
			sampler = new JitteredSampler(num_samples);
		}
		else if (!strcmp(argv[i], "-render_filter")) {

			i++; assert(i < argc);
			filter_file = argv[i];
			i++; assert(i < argc);
			filter_zoom = atoi(argv[i]);
			render_filter = true;
		}
		else if (!strcmp(argv[i], "-box_filter")) {
			i++; assert(i < argc);
			float r = atof(argv[i]);
			filter = new BoxFilter(r);
		}
		else if (!strcmp(argv[i], "-tent_filter")) {
			i++; assert(i < argc);
			float r = atof(argv[i]);
			filter = new TentFilter(r);
		}
		else if (!strcmp(argv[i], "-gaussian_filter")) {
			i++; assert(i < argc);
			float r = atof(argv[i]);
			filter = new GaussianFilter(r);
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
	parser = new SceneParser(input_file);
	if (grid_used)
	{
		BoundingBox* boundingbox;
		boundingbox = parser->getGroup()->getBoundingBox();
		grid = new Grid(boundingbox, nx, ny, nz);
		//At first In my design, I set matrix as E
		//But there's a big problem that every ray would be transformed in this case
		//So it should be nullptr 
		parser->getGroup()->insertIntoGrid(grid, nullptr);
	}
	//Main Code Part
	if (gui_used)
	{
		GLCanvas glcanvas;
		glcanvas.initialize(parser, shade, trace, grid, visualize_grid);
	}
	else
	{
		shade();
	}
}

void shade()
{
	Image out_image(width, height);
	Film film(width, height, num_samples);
	Camera* camera = parser->getCamera();
	float tmin = camera->getTMin();
	RayTracer raytracer(parser, max_bounces, cutoff_weight, shadows, grid);
	//raytracing status initialize
	RayTracingStats::Initialize(width, height, parser->getGroup()->getBoundingBox(), nx, ny, nz);
	//RayTracer raytracer(parser, max_bounces, cutoff_weight, shadows);
	for (int i = 0; i < out_image.Width(); i++)
	{
		for (int j = 0; j < out_image.Height(); j++)
			for (int num = 0; num < num_samples; num++)
			{
				Vec2f pos = (sampler) ? sampler->getSamplePosition(num) : Vec2f(0, 0); //get sample position
				float x_bias = (i - out_image.Width() * 1.0 / 2 + pos.x()) / out_image.Width();
				float y_bias = (j - out_image.Height() * 1.0 / 2 + pos.y()) / out_image.Height();
				Ray ray = camera->generateRay(Vec2f(x_bias, y_bias));
				Hit hit;
				Vec3f color = raytracer.traceRay(ray, tmin, 0, 0, 1, hit);
				if (sampler && filter)
					film.setSample(i, j, num, pos, color);
				else
					out_image.SetPixel(i, j, color);
			}
	}
	if (sampler && filter)
	{
		for (int i = 0; i < out_image.Width(); i++)
		{
			for (int j = 0; j < out_image.Height(); j++)
			{
				Vec3f color = filter->getColor(i, j, &film);
				out_image.SetPixel(i, j, color);
			}
		}
	}

	if (render_samples)
	{
		film.renderSamples(sample_file, sample_zoom);
	}
	
	if (render_filter)
	{
		film.renderFilter(filter_file, filter_zoom, filter);
	}
	
	if (tracing_stats)
		RayTracingStats::PrintStatistics();
	if (output_file)
		out_image.SaveTGA(output_file);
}

void trace(float x, float y)
{
	Camera* camera = parser->getCamera();
	float tmin = camera->getTMin();
	//RayTracer raytracer(parser, max_bounces, cutoff_weight, shadows);
	RayTracer raytracer(parser, grid);
	Vec2f point(x - 0.5, y - 0.5);
	Ray ray = camera->generateRay(point);
	Hit hit_result;
	Vec3f Color = raytracer.traceRay(ray, camera->getTMin(), 0, cutoff_weight, 0, hit_result);
}

Vec3f mirrorDirection(const Vec3f& normal, const Vec3f& incoming)
{
	Vec3f n = normal, v = incoming;
	n.Normalize();
	v.Normalize();
	Vec3f mirror = v - 2 * n.Dot3(v) * n;
	mirror.Normalize();
	return	mirror;
}

bool transmittedDirection(const Vec3f& normal, const Vec3f& incoming,
	float index_i, float index_t, Vec3f& transmitted)
{
	Vec3f n = normal, i = incoming;
	n.Normalize();
	i.Normalize();
	float cos_theta = n.Dot3(i);
	Vec3f m = -cos_theta * n + i;
	float in_sin = m.Length();
	float out_sin = in_sin * index_i / index_t;
	if (out_sin > 1)
		return false;
	float out_cos = sqrt(1 - out_sin * out_sin);
	m.Normalize();
	if (cos_theta < 0)
		transmitted = -out_cos * n + out_sin * m;
	else
		transmitted = out_cos * n + out_sin * m;
	return true;
}