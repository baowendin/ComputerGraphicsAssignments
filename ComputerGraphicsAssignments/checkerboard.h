#pragma once
#include "material.h"
#include "matrix.h"
#include<math.h>
class CheckerBoard : public Material
{
	Matrix* m;
	Material* material1, * material2;
	CheckerBoard(Matrix* m, Material* mat1, Material* mat2) : Material(Vec3f(1, 1, 1))
	{
		this->m = m;
		this->material1 = material1;
		this->material2 = material2;
	}

	Vec3f getSpecularColor() const { return material1->getSpecularColor(); }

	Vec3f getRedlectiveColor() const { return material1->getReflectiveColor(); }

	Vec3f getTransparentColor() const { return material1->getTransparentColor(); }

	float getIndexOfRef() const { return material1->getIndexOfRefraction(); }

	void glSetMaterial(void) const { material1->glSetMaterial(); }

	Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const
	{
		Vec3f intersection = hit.getIntersectionPoint();
		m->Transform(intersection);
		bool is_odd = (int)(floor(intersection[0]) + floor(intersection[1]) + floor(intersection[2])) % 2;
		//I'm not sure odd corresponding to which
		if (is_odd)
		{
			return material1->Shade(ray, hit, dirToLight, lightColor);
		}
		else
		{
			return material2->Shade(ray, hit, dirToLight, lightColor);
		}
		return Vec3f();
	}


};