#pragma once
#include "object3d.h"
#include "matrix.h"
class Triangle : public Object3D
{
	Vec3f a, b, c, normal;
public:
	Triangle(Vec3f& a, Vec3f& b, Vec3f& c, Material* m)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->material = m;
		Vec3f::Cross3(this->normal, b - a, c - b);
		this->normal.Normalize();
	}

	bool intersect(const Ray& r, Hit& h, float tmin)
	{
		Vec3f direction = r.getDirection(), origin = r.getOrigin();
		float A = det3x3(
			a.x() - b.x(), a.x() - c.x(), direction.x(),
			a.y() - b.y(), a.y() - c.y(), direction.y(),
			a.z() - b.z(), a.z() - c.z(), direction.z()
		);
		if (A == 0)
			return false;
		float beta = det3x3(
			a.x() - origin.x(), a.x() - c.x(), direction.x(),
			a.y() - origin.y(), a.y() - c.y(), direction.y(),
			a.z() - origin.z(), a.z() - c.z(), direction.z()
		) / A;

		float gama = det3x3(
			a.x() - b.x(), a.x() - origin.x(), direction.x(),
			a.y() - b.y(), a.y() - origin.y(), direction.y(),
			a.z() - b.z(), a.z() - origin.z(), direction.z()
		) / A;

		float t = det3x3(
			a.x() - b.x(), a.x() - c.x(), a.x() - origin.x(),
			a.y() - b.y(), a.y() - c.y(), a.y() - origin.y(),
			a.z() - b.z(), a.z() - c.z(), a.z() - origin.z()
		) / A;
		bool res = false;
		if (beta >= 0 && gama >= 0 && beta + gama <= 1 && t >= tmin)
		{
			res = true;
		}
		if (res)
		{
			h.set(t, this->material, this->normal, r);
		}
		return res;
		
	}
};
