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
		this->direction.Normalize();
		Vec3f::Cross3(this->hor, this->direction, this->up);
		this->hor.Normalize();
		Vec3f::Cross3(this->up, this->hor, this->direction);
		this->up.Normalize();
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

	void glInit(int w, int h)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(angle * 180.0 / 3.14159, (float)w / float(h), 0.5, 40.0);
	}

	void glPlaceCamera(void)
	{
		gluLookAt(center.x(), center.y(), center.z(),
			center.x() + direction.x(), center.y() + direction.y(), center.z() + direction.z(),
			up.x(), up.y(), up.z());
	}

	void dollyCamera(float dist)
	{
		center += direction * dist;

		// ===========================================
		// ASSIGNMENT 3: Fix any other affected values
		// ===========================================
	}

	void truckCamera(float dx, float dy)
	{
		Vec3f horizontal;
		Vec3f::Cross3(horizontal, direction, up);
		horizontal.Normalize();

		Vec3f screenUp;
		Vec3f::Cross3(screenUp, horizontal, direction);

		center += horizontal * dx + screenUp * dy;
		// ===========================================
		// ASSIGNMENT 3: Fix any other affected values
		// ===========================================
	}

	void rotateCamera(float rx, float ry)
	{
		Vec3f horizontal;
		Vec3f::Cross3(horizontal, direction, up);
		horizontal.Normalize();

		// Don't let the model flip upside-down (There is a singularity
		// at the poles when 'up' and 'direction' are aligned)
		float tiltAngle = acos(up.Dot3(direction));
		if (tiltAngle - ry > 3.13)
			ry = tiltAngle - 3.13;
		else if (tiltAngle - ry < 0.01)
			ry = tiltAngle - 0.01;

		Matrix rotMat = Matrix::MakeAxisRotation(up, rx);
		rotMat *= Matrix::MakeAxisRotation(horizontal, ry);

		rotMat.Transform(center);
		rotMat.TransformDirection(direction);
		direction.Normalize();

		// ===========================================
		// ASSIGNMENT 3: Fix any other affected values
		// ===========================================
		Vec3f::Cross3(hor, up, direction);
		hor.Normalize();
		/*
		Vec3f::Cross3(up, hor, direction);
		up.Normalize();
		*/

	}

};