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
#endif // !_OBJECT3D_H
