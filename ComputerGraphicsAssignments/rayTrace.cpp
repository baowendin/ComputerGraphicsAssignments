#include "rayTrace.h"
void trace()
{

}
void ray_trace(int argc, char** argv)
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
	bool shade_back = false, gui_used = false, gouraud_used = false;
	float theta, phi;
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
			theta = atof(argv[i]);
			i++; assert(i < argc);
			phi = atof(argv[i]);

		}
		else if (!strcmp(argv[i], "-gouraud")) {
			gouraud_used = true;
		}
		else {
			printf("whoops error with command line argument %d: '%s'\n", i, argv[i]);
			assert(0);
		}
	}
	SceneParser parser(input_file);
	//Main Code Part
	if (gui_used)
	{
		GLCanvas glcanvas;
		glcanvas.initialize(&parser, trace);
	}
	else
	{
		//OTHER PART, NOT THINK ABOUT IT YET
	}
}