#pragma once
#include "object3d.h"
#include<vector>
using namespace std;

class Group : public Object3D
{
	vector<Object3D*> object_group;
	int count;
public:
	Group(int count)
	{
		object_group.clear();
		this->count = count;
	}

	void addObject(int num, Object3D* object)
	{
		object_group.push_back(object);
		// extend the boundingbox part
		if (!object->getBoundingBox())
			return;
		if (!boundingbox)
		{
			boundingbox = new BoundingBox(object->getBoundingBox()->getMin(), object->getBoundingBox()->getMax());
		}
		else
		{
			boundingbox->Extend(object->getBoundingBox());
		}
			
	}

	bool intersect(const Ray& r, Hit& h, float tmin)
	{
		bool mark = false;
		for(auto& object : object_group)
		{
			Hit hit;
			if (object->intersect(r, hit, tmin))
			{
				if (!mark || hit.getT() < h.getT())
				{									
					h = hit;
				}
				mark = true;			
			}
		}
		return mark;
	}
	
	void paint()
	{
		for (auto& object : object_group)
			object->paint();
	}

	void insertIntoGrid(Grid* g, Matrix* m)
	{
		for (auto& object : object_group)
		{
			object->insertIntoGrid(g, m);
		}
	}
};
