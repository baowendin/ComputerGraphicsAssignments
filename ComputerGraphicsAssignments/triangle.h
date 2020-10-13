#pragma once
#include "object3d.h"
#include "matrix.h"
#include <algorithm>
#include "boundingbox.h"
#define EPSILON 0.0001
class Triangle : public Object3D
{
	Vec3f a, b, c, normal;

	BoundingBox* calculate_boundingbox()
	{
		BoundingBox* boundingbox = new BoundingBox(a, a);
		boundingbox->Extend(b);
		boundingbox->Extend(c);
		return boundingbox;
	}
public:
	Triangle(Vec3f& a, Vec3f& b, Vec3f& c, Material* m)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		this->material = m;
		this->boundingbox = calculate_boundingbox();
		Vec3f::Cross3(this->normal, b - a, c - b);
		this->normal.Normalize();
	}

	bool intersect(const Ray& r, Hit& h, float tmin)
	{
		RayTracingStats::IncrementNumIntersections();
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

	void paint()
	{
		this->material->glSetMaterial();
		glBegin(GL_TRIANGLES);
		glNormal3f(normal.x(), normal.y(), normal.z());
		glVertex3f(a.x(), a.y(), a.z());
		glVertex3f(b.x(), b.y(), b.z());
		glVertex3f(c.x(), c.y(), c.z());	
		glEnd();
	}

	void insertIntoGrid(Grid* g, Matrix* m)
	{
		BoundingBox* grid_box = g->getBoundingBox();
		Vec3f minimum = boundingbox->getMin() - grid_box->getMin();
		Vec3f maximum = boundingbox->getMax() - grid_box->getMin();

		minimum.Divide(g->get_cell_size());
		maximum.Divide(g->get_cell_size());

		maximum += Vec3f(EPSILON, EPSILON, EPSILON);
		minimum -= Vec3f(EPSILON, EPSILON, EPSILON);
		//Not same as Sphere, in this case we have no optimization
		//We just insert the boundingbox into grid
		for (int i = max(0.f, floor(minimum.x())); i < min(1.f * g->nx, ceil(maximum.x())); i++)
		{
			for (int j = max(0.f, floor(minimum.y())); j < min(1.f * g->ny, ceil(maximum.y())); j++)
			{
				for (int k = max(0.f, floor(minimum.z())); k < min(1.f * g->nz, ceil(maximum.z())); k++)
				{
					g->addObject(i, j, k, this);
				}
			}
		}
	}

};
