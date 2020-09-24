#pragma once
#include "image.h"
#include "hit.h"
#include "light.h"
#include "group.h"
#include "material.h"
#include "scene_parser.h"
#include "orthographic.h"
#include "camera.h"
#include "glCanvas.h"
#include<iostream>
#include<string.h>
#include<assert.h>
extern bool shade_back, gui_used, gouraud_used;
extern int theta_num, phi_num;
void ray_trace(int argc, char** argv);