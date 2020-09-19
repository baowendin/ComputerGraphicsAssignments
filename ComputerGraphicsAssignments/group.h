#pragma once
#include "object3d.h"
#include<vector>

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

};
