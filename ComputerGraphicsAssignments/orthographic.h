#pragma once
#include "vectors.h"
#include "camera.h"
#include "ray.h"
class OrthographicCamera : public Camera
{
	Vec3f center;
	Vec3f direction;
	Vec3f up;
	float size;
public:
	OrthographicCamera(Vec3f center, Vec3f direction, Vec3f up, float size)
	{
		this->center = center;
		this->direction = direction;
		direction.Normalize();
		Vec3f tmp;
		Vec3f::Cross3(tmp, direction, up);
		Vec3f::Cross3(this->up, tmp, direction);
		this->size = size;
	}

	Ray generateRay(Vec2f point)
	{
		Vec3f hor, ver;		
		Vec3f::Cross3(hor, direction, up);
		ver = up;
		hor.Normalize();
		ver.Normalize();				
		float ch = size * point.x();
		float cw = size * point.y();
		hor.Scale(ch, ch, ch);
		ver.Scale(cw, cw, cw);
		Vec3f from_point = center + hor + ver;
		return Ray(from_point, direction);
	}
	
	float getTMin() const
	{
		return (float)INT_MIN;
	}
};
