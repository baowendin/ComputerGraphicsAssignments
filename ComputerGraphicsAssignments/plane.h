#pragma once
#include "object3d.h"
#define INF 10000
class Plane : public Object3D 
{
	Vec3f normal;
	float d;
public:
	bool intersect(const Ray& r, Hit& h, float tmin)
	{
		RayTracingStats::IncrementNumIntersections();
		if (normal.Dot3(r.getDirection()) == 0)
		{
			if (normal.Dot3(r.getOrigin()) != d)
				return false;
			else
			{
				Vec3f tmp = normal;
				tmp.Normalize();
				h.set(0, this->material, tmp, r);
				return true;
			}
				
		}			
		Vec3f tmp = normal;
		tmp.Normalize();
		float t = (d - r.getOrigin().Dot3(normal)) / r.getDirection().Dot3(normal);
		h.set(t, this->material, tmp, r);
		return tmin <= t;
	}

	Plane(Vec3f& normal, float d, Material* m)
	{
		this->normal = normal;
		this->d = d;
		this->material = m;
	}

	void paint()
	{
		//Any vector that is not parallel to normal is ok
		Vec3f tmp = normal == Vec3f(1, 0, 0) ? Vec3f(0, 1, 0) : Vec3f(1, 0, 0);
		Vec3f b1, b2;
		Vec3f::Cross3(b1, normal, tmp);
		Vec3f::Cross3(b2, b1, normal);
		Vec3f center;
		//We get a center, center dot normal = d
		if (normal.x() != 0)
			center = Vec3f(d / normal.x(), 0, 0);
		else if (normal.y() != 0)
			center = Vec3f(0, d / normal.y(), 0);
		else if (normal.z() != 0)
			center = Vec3f(0, 0, d / normal.z());
		else
		{
			cout << "The normal is wrongly set to (0, 0, 0)";
			assert(0);
		}
		Vec3f a = center - INF * b1 - INF * b2;
		Vec3f b = center - INF * b1 + INF * b2;
		Vec3f c = center + INF * b1 + INF * b2;
		Vec3f d = center + INF * b1 - INF * b2;
		this->material->glSetMaterial();
		glBegin(GL_QUADS);
		glNormal3f(normal.x(), normal.y(), normal.z());
		glVertex3f(a.x(), a.y(), a.z());
		glVertex3f(b.x(), b.y(), b.z());
		glVertex3f(c.x(), c.y(), c.z());
		glVertex3f(d.x(), d.y(), d.z());
		glEnd();
	}
};
