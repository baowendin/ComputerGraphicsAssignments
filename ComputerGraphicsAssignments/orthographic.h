#pragma once
#include "vectors.h"
#include "camera.h"
#include "ray.h"
#include "matrix.h"
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
	// OpenGL
	void glInit(int w, int h)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (w > h)
			glOrtho(-size / 2.0, size / 2.0, -size * (float)h / (float)w / 2.0, size * (float)h / (float)w / 2.0, 0.5, 40.0);
		else
			glOrtho(-size * (float)w / (float)h / 2.0, size * (float)w / (float)h / 2.0, -size / 2.0, size / 2.0, 0.5, 40.0);
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
	}

	void truckCamera(float dx, float dy)
	{
		Vec3f horizontal;
		Vec3f::Cross3(horizontal, direction, up);
		horizontal.Normalize();

		Vec3f screenUp;
		Vec3f::Cross3(screenUp, horizontal, direction);

		center += horizontal * dx + screenUp * dy;
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
		// I think the up should also update, otherwise there maybe problems
		/*
		Vec3f::Cross3(horizontal, direction, up);
		horizontal.Normalize();
		Vec3f::Cross3(up, horizontal, direction);
		up.Normalize();
		*/
		
	}
};
