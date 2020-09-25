#pragma once
#include "image.h"
#include "rayTree.h"
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
#define EPSILON 0.01
extern bool shade_back, gui_used, gouraud_used;
extern int theta_num, phi_num;
void ray_trace(int argc, char** argv);
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

}

class RayTracer
{
	SceneParser* parser;
	int max_bounces;
	float cutoff_weight;
	bool shadows;
public:
	RayTracer(SceneParser* s, int max_bounces, float cutoff_weight, bool shadows, ...)
	{
		this->parser = s;
		this->max_bounces = max_bounces;
		this->cutoff_weight = cutoff_weight;
		this->shadows = shadows;
	}

	Vec3f traceRay(Ray& ray, float tmin, int bounces, float weight,
		float indexOfRefraction, Hit& hit) const
	{
		if (bounces > max_bounces)
			return Vec3f(0, 0, 0);
		Vec3f phong_color;
		Object3D* group = parser->getGroup();
		if (group->intersect(ray, hit, tmin))
		{
			if (bounces == 0) RayTree::SetMainSegment(ray, 0, hit.getT());
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
					//cout << dir << endl;
					if (group->intersect(light_ray, light_hit, EPSILON))
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
						//TODO: RayTree
					}
				}
				color += flag * hit.getMaterial()->Shade(ray, hit, dir, col);
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

 			}			
			return color;
		}
		else if (bounces == 0)
		{
			RayTree::SetMainSegment(ray, 0, 10);
			// if no intersect, return the backgroug Color
		}
			return parser->getBackgroundColor();
	}
};