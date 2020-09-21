#pragma once
#include "object3d.h"
class Plane : public Object3D 
{
	Vec3f normal;
	float d;
public:
	bool intersect(const Ray& r, Hit& h, float tmin)
	{
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
};
