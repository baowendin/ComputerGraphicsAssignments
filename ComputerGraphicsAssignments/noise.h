#pragma once
#include "material.h"
#include "matrix.h"
#include<math.h>
#include"perlin_noise.h"
class Noise : public Material
{
	Matrix* m;
	Material* material1, * material2;
	int octaves;

public:
	Noise(Matrix* m, Material* mat1, Material* mat2, int octaves) : Material(Vec3f(1, 1, 1))
	{
		this->material1 = mat1;
		this->material2 = mat2;
		this->m = m;
		this->octaves = octaves;
	}
	
	Vec3f getSpecularColor() const { return material1->getSpecularColor(); }

	Vec3f getReflectiveColor() const const { return material1->getReflectiveColor(); }

	Vec3f getTransparentColor() const { return material1->getTransparentColor(); }

	float getIndexOfRefraction() const { return material1->getIndexOfRefraction(); }

	void glSetMaterial(void) const { material1->glSetMaterial(); }

	Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const
	{
		Vec3f intersection = hit.getIntersectionPoint();
		m->Transform(intersection);
		float exponent = PerlinNoise::getNoise(intersection, octaves);
		return (1 - exponent) * material2->Shade(ray, hit, dirToLight, lightColor) + exponent * material1->Shade(ray, hit, dirToLight, lightColor);
	}
};

class Marble : public Material{
	Matrix* m;
	Material* material1, * material2;
	int octaves;
	float frequency, amplitude;
public:
	Marble(Matrix* m, Material* mat1, Material* mat2, int octaves, float frequency, float amplitude) : Material(Vec3f(1, 1, 1))
	{
		this->material1 = mat1;
		this->material2 = mat2;
		this->m = m;
		this->octaves = octaves;
		this->frequency = frequency;
		this->amplitude = amplitude;
	}

	Vec3f getSpecularColor() const { return material1->getSpecularColor(); }

	Vec3f getReflectiveColor() const const { return material1->getReflectiveColor(); }

	Vec3f getTransparentColor() const { return material1->getTransparentColor(); }

	float getIndexOfRefraction() const { return material1->getIndexOfRefraction(); }

	void glSetMaterial(void) const { material1->glSetMaterial(); }

	Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const
	{
		Vec3f intersection = hit.getIntersectionPoint();
		m->Transform(intersection);
		float exponent = sin(frequency * intersection.x() + amplitude * PerlinNoise::getNoise(intersection, octaves));
		return (1 - exponent) * material2->Shade(ray, hit, dirToLight, lightColor) + exponent * material1->Shade(ray, hit, dirToLight, lightColor);
	}
};