#pragma once
#include "camera.h"
class PerspectiveCamera : public Camera
{
	Vec3f center, direction, up, hor;
	float angle;
public:
	PerspectiveCamera(Vec3f center, Vec3f& direction, Vec3f& up, float angle)
	{
		this->center = center;
		this->direction = direction;
		this->up = up;
		Vec3f::Cross3(this->hor, this->direction, this->up);
		this->direction.Normalize();
		this->up.Normalize();
		this->hor.Normalize();
		this->angle = angle;
	}

	virtual Ray generateRay(Vec2f point)
	{
		Vec3f temp = direction + 2 * tan(angle / 2) * up *point.y() + 2 * tan(angle / 2) * hor * point.x();
		return Ray(center, temp);
	}

	virtual float getTMin() const 
	{
		return (float)0;
	}
};