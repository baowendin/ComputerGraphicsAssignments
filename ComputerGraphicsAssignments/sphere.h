#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "object3d.h"
#include "rayTrace.h"
class Sphere : public Object3D
{
	Vec3f center;
	float radius;

	BoundingBox* calculate_boundingbox()
	{
		return new BoundingBox(Vec3f(center.x() - radius, center.y() - radius, center.z() - radius), Vec3f(center.x() + radius, center.y() + radius, center.z() + radius));
	}
public:
	Sphere() {}

	Sphere(Vec3f c, double r, Material* material)
	{
		this->center = c;
		this->radius = r;
		this->material = material;
		this->boundingbox = calculate_boundingbox();
	}

	bool intersect(const Ray& r, Hit& h, float tmin)
	{
		Vec3f source = r.getOrigin();
		source -= center;
		//cout << source << endl;
		float a = r.getDirection().Dot3(r.getDirection());
		float b = r.getDirection().Dot3(source) * 2;
		float c = source.Dot3(source) - radius * radius;
		//cout << a  << " " << b << " " << c << " " << endl;
		float d2 = b * b - 4 * a * c;
		if (d2 < 0)
		{
			//cout << "fuck";
			return false;
		}

		float t1 = (-b - (float)sqrt(d2)) / (2 * a);
		float t2 = (-b + (float)sqrt(d2)) / (2 * a);
		if (tmin < t1)
		{
			Vec3f hit_point = r.pointAtParameter(t1);
			Vec3f normal = hit_point - center;
			normal.Normalize();
			h.set(t1, this->material, normal, r);
			return true;
		}
		else if (tmin <= t2)
		{
			Vec3f hit_point = r.pointAtParameter(t2);
			Vec3f normal = hit_point - center;
			normal.Normalize();
			h.set(t2, this->material, normal, r);
			return true;
		}
		return false;



		//NOT USED
		//以下为错误的几何法内容，无法处理t为negative或者点在球内的特殊情况
		/*Vec3f dis_to_center = r.getOrigin() - center;
		float dis_cast = (float)abs(dis_to_center.Dot3(r.getDirection()) / r.getDirection().Length());
		float ray_center_dis = (float)sqrt(dis_to_center.Length() * dis_to_center.Length() - dis_cast * dis_cast);
		if (ray_center_dis > radius)
			return false;
		float dis_center_length = dis_to_center.Length();
		float bias = (float)sqrt(radius * radius - ray_center_dis * ray_center_dis);
		//cout << dis_cast << " " << bias << endl;
		if (tmin < dis_cast - bias)
		{
			h.set(dis_cast - bias, this->material, r);
			return true;
		}
		else if (tmin <= dis_cast + bias)
		{
			h.set(dis_cast + bias, this->material, r);
			return true;
		}
		return false;*/
	}

	void paint();
	void insertIntoGrid(Grid* g, Matrix* m);
	
};
