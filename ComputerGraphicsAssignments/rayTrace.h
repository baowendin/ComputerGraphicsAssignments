#pragma once
#include "image.h"
#include "rayTree.h"
#include "hit.h"
#include "light.h"
#include "group.h"
#include "scene_parser.h"
#include "orthographic.h"
#include "camera.h"
#include "glCanvas.h"
#include "material.h"
#include<iostream>
#include<string.h>
#include<assert.h>
#include "raytracing_stats.h"
#include"grid.h"
#include "film.h"
#include "sampler.h"
#include "filter.h"
#define EPSILON 0.01
extern bool shade_back, gui_used, gouraud_used, visualize_grid;
extern int theta_num, phi_num;

void ray_trace(int argc, char** argv);
Vec3f mirrorDirection(const Vec3f& normal, const Vec3f& incoming);
bool transmittedDirection(const Vec3f& normal, const Vec3f& incoming, float index_i, float index_t, Vec3f& transmitted);
class RayTracer
{
	SceneParser* parser;
	int max_bounces;
	float cutoff_weight;
	bool shadows;
	Grid* grid;
public:
	RayTracer(SceneParser* s, int max_bounces, float cutoff_weight, bool shadows, Grid* grid ...)
	{
		this->parser = s;
		this->max_bounces = max_bounces;
		this->cutoff_weight = cutoff_weight;
		this->shadows = shadows;
		this->grid = grid;
	}
	//Second Mode
	RayTracer(SceneParser* s, Grid* grid)
	{
		this->parser = s;
		this->grid = grid;
		this->shadows = false;
		this->max_bounces = 0;
	}

	bool get_intersect(Object3D* group, Ray& ray, Hit& hit, float tmin) const
	{
		if (grid && !visualize_grid)
			return grid->intersect_real(ray, hit, tmin);
		else
			return group->intersect(ray, hit, tmin);
	}

	Vec3f traceRay(Ray& ray, float tmin, int bounces, float weight,
		float indexOfRefraction, Hit& hit) const
	{
		if (bounces > max_bounces)
			return Vec3f(0, 0, 0);
		Vec3f phong_color;
		Vec3f sum_up;
		Object3D* group = parser->getGroup();
		if (grid)
		{
			group = grid;
		}
		if (get_intersect(group, ray, hit, tmin))
		{
			if (grid || bounces == 0) RayTree::SetMainSegment(ray, 0, hit.getT());
			Vec3f color = parser->getAmbientLight() * hit.getMaterial()->getDiffuseColor(); // Ambient Part
			//然后考虑全局光照、反射，折射三个方向
			//全局光照
			for (int i = 0; i < parser->getNumLights(); i++)
			{
				Light* light = parser->getLight(i);
				Vec3f p, dir, col;
				Vec3f flag(1, 1, 1);
				p = hit.getIntersectionPoint();
				float dis;
				light->getIllumination(p, dir, col, dis);
				if (shadows)
				{
					Hit light_hit;
					Ray light_ray(p, dir);
					RayTracingStats::IncrementNumShadowRays();
 					if (get_intersect(group, light_ray, light_hit, EPSILON))
					{
						if (dis > light_hit.getT())
						{
							flag = Vec3f(0, 0, 0);
							RayTree::AddShadowSegment(light_ray, 0, light_hit.getT());
						}
						else
						{
							RayTree::AddShadowSegment(light_ray, 0, dis);
						}
					}
					else
					{
						RayTree::AddShadowSegment(light_ray, 0, dis);
					}
				}
				else
					RayTracingStats::IncrementNumNonShadowRays();
				color += flag * hit.getMaterial()->Shade(ray, hit, dir, col);
			}
			//反射
			if (hit.getMaterial()->getReflectiveColor().Length() > 0)
			{
				Vec3f reflect = mirrorDirection(hit.getNormal(), ray.getDirection());
				Ray reflect_ray = Ray(hit.getIntersectionPoint(), reflect);
				Hit reflect_hit;
				color += traceRay(reflect_ray, EPSILON, bounces + 1, weight, indexOfRefraction, reflect_hit) * hit.getMaterial()->getReflectiveColor();
				RayTree::AddReflectedSegment(reflect_ray, 0, reflect_hit.getT());
			}
			//折射
			if (hit.getMaterial()->getTransparentColor().Length() > 0)
			{
				bool flag;
				Vec3f transmitted;
				float to_index;
				if (ray.getDirection().Dot3(hit.getNormal()) < 0)
					to_index = hit.getMaterial()->getIndexOfRefraction();
				else
					to_index = 1;
				flag = transmittedDirection(hit.getNormal(), ray.getDirection(), indexOfRefraction, to_index, transmitted);
				if (flag)
				{
					Ray transmitted_ray(hit.getIntersectionPoint(), transmitted);
					Hit transmitted_hit;
					Vec3f tmp = traceRay(transmitted_ray, EPSILON, bounces + 1, weight, to_index, transmitted_hit) * hit.getMaterial()->getTransparentColor();
					sum_up += tmp;
					color += tmp;
					RayTree::AddTransmittedSegment(transmitted_ray, 0, transmitted_hit.getT());
				}
			}
			//cout << sum_up << endl;
			return color;
		}
		else if (grid || bounces == 0)
		{
			RayTree::SetMainSegment(ray, 0, 10);
			// if no intersect, return the backgroug Color
		}
		return parser->getBackgroundColor();
	}
};
