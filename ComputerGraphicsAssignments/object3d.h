#ifndef _OBJECT3D_H
#define _OBJECT3D_H
#include"material.h"
#include"ray.h"
#include"hit.h"
class Object3D
{
protected:
	Material* material;
public:
	virtual bool intersect(const Ray& r, Hit& h, float tmin) = 0;
	Object3D(){}
	void set_material(Material* m)
	{
		material = m;
	}

	Material* get_material()
	{
		return material;
	}

	virtual ~Object3D()
	{
		delete material;
	}
};

class Group : public Object3D
{

};

class Sphere : public Object3D
{
	Vec3f center;
	double radius;
public:
	Sphere(){}
	Sphere(Vec3f c, double r)
	{
		center = c;
		radius = r;
	}
};
#endif // !_OBJECT3D_H
