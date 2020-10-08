#ifndef _OBJECT3D_H
#define _OBJECT3D_H
#include"material.h"
#include"ray.h"
#include"hit.h"
#include "boundingbox.h"
class Object3D
{
protected:
	Material* material;
	BoundingBox* boundingbox;
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
		// if case of special case like plane;
		if (boundingbox) 
			delete boundingbox;
	}
	virtual void paint() = 0;


};
#endif // !_OBJECT3D_H
