#pragma once
#include "object3d.h"
class Transform : public Object3D
{
	Matrix m;
	Object3D* o;
	Vec3f get_vec3f(Vec4f v)
	{
		return Vec3f(v.x(), v.y(), v.z());
	}
public:
	Transform(Matrix& m, Object3D* o)
	{
		this->m = m;
		this->o = o;
	}
	// Transform the ray, then delegate intersect it to Object3D
	virtual bool intersect(const Ray& r, Hit& h, float tmin)
	{
		// Transform ray to object scene
		Vec3f direction = r.getDirection();
		Vec3f origin = r.getOrigin();
		Matrix m_inverse = m;
		// using M^-1 to transform
		m_inverse.Inverse();
		m_inverse.TransformDirection(direction);
		m_inverse.Transform(origin);
		if (o->intersect(Ray(origin, direction), h, tmin))
		{
			Matrix m_inverse_transpose = m_inverse;
			m_inverse_transpose.Transpose();
			Vec3f normal = h.getNormal();
			m_inverse_transpose.TransformDirection(normal);
			normal.Normalize();
			h.set(h.getT(), h.getMaterial(), normal, r);
			//cout << h.getT() << endl;
			return true;
		}
		else
			return false;
	}

	void paint()
	{
		glPushMatrix();
		GLfloat* glMatrix = m.glGet();
		glMultMatrixf(glMatrix);
		delete[] glMatrix;
		o->paint();
		glPopMatrix();

	}
};
