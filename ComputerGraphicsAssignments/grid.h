#pragma once
#define MAX_COLOR_SIZE 10
#define EPSILON 0.001 //float 6位有效数字，所以取0.001还是比较保守的

#include "object3d.h"
#include"rayTree.h"
#include "material.h"
#include "boundingbox.h"
#include "material.h"
#include<vector>
#include<unordered_set>
using namespace std;
class MarchingInfo
{
public:
	float tmin;
	int cur_x, cur_y, cur_z;
	int next_x, next_y, next_z;
	bool no_intersection;
	Vec3f pos;
	Vec3f normal;
	Vec3f direction_sign;
	Vec3f error_term;
	Vec3f dt; // [0, 1] for each axis and normalized

	void update_index()
	{
		cur_x = int(pos.x());
		cur_y = int(pos.y());
		cur_z = int(pos.z());
	}

	void next_cell()
	{
		RayTracingStats::IncrementNumGridCellsTraversed();
		//find the next cell
		Vec3f dire = Vec3f(error_term.x() < min(error_term.y(), error_term.z()), error_term.y() < min(error_term.x(), error_term.z()), error_term.z() < min(error_term.x(), error_term.y()));
		//dire.length() should be 1, but there are special cases that it's too close to distinguish using float
		//we just make them look different, even it's wrong 
		if (dire.Length() < 0.1 || dire.Length() > 1.1)
		{
			if (error_term.x() == error_term.y() && error_term.x() == error_term.z())
			{
				if (dt.x() > dt.y() && dt.x() > dt.z())
					dire = Vec3f(1, 0, 0);
				else if (dt.y() > dt.x() && dt.y() > dt.z())
					dire = Vec3f(0, 1, 0);
				else
					dire = Vec3f(0, 0, 1);
			}
			else if (error_term.x() == error_term.y())
			{
				if (dt.x() > dt.y())
					dire = Vec3f(1, 0, 0);
				else
					dire = Vec3f(0, 1, 0);
			}
			else if (error_term.x() == error_term.z())
			{
				if (dt.x() > dt.z())
					dire = Vec3f(1, 0, 0);
				else
					dire = Vec3f(0, 0, 1);
			}
			else
			{
				if (dt.y() > dt.z())
					dire = Vec3f(0, 1, 0);
				else
					dire = Vec3f(0, 0, 1);
			}
		}
		//If still (that's almost possible
		//TODO: set it with a value
		normal = dire * direction_sign * -1;
		error_term += dire * direction_sign / dt;
		pos += dire * direction_sign;
		update_index();
	}
};
class Grid : public Object3D
{
protected:
	vector<vector<Object3D*> > record;
	vector<Object3D*> infinite_object_list;
	Vec3f cell_size;
	Material* color_collection[10];
	vector<Object3D*>* get_record(int i, int j, int k)
	{
		if (i < 0 || i >= nx)
			return NULL;
		if (j < 0 || j >= ny)
			return NULL;
		if (k < 0 || k >= nz)
			return NULL;
		return &record[i * ny * nz + j * nz + k];
	}

	void init_visual_color()
	{
		color_collection[0] = new PhongMaterial(Vec3f(1, 1, 1));
		for (int i = 1; i < MAX_COLOR_SIZE; i++)
		{
			Vec3f color = Vec3f(rand() % 256, rand() % 256, rand() % 256) * (1. / 256.);
			color_collection[i] = new PhongMaterial(color);
		}
	}
public:
	int nx, ny, nz;

	Grid(BoundingBox* bb, int nx, int ny, int nz)
	{
		this->boundingbox = bb;
		this->nx = nx;
		this->ny = ny;
		this->nz = nz;
		record.resize(nx * ny * nz);
		cell_size = bb->getMax() - bb->getMin();
		cell_size.Divide(nx, ny, nz);
		Vec3f tmp_color(0, 0, 0);
		this->material = new PhongMaterial(Vec3f(1, 1, 1));
		init_visual_color();
	}

	Vec3f get_center(int x, int y, int z)
	{
		return boundingbox->getMin() + (Vec3f(x, y, z) + Vec3f(0.5, 0.5, 0.5)) * cell_size;
	}

	Vec3f get_cell_size()
	{
		return cell_size;
	}

	float get_halfdialog()
	{
		return cell_size.Length() / 2;
	}

	void addObject(int i, int j, int k, Object3D* object)
	{
		get_record(i, j, k)->push_back(object);
	}
	//画6个面，比较hardcode就不做修改了
	Vec3f getNormal(int type)
	{
		static Vec3f nor[] = { Vec3f(1,0,0),Vec3f(-1,0,0),Vec3f(0,1,0),Vec3f(0,-1,0),
			Vec3f(0,0,1),Vec3f(0,0,-1) };
		return nor[type];
	}

	vector<Vec3f> getFace(int x, int y, int z, int type)
	{
		Vec3f r = boundingbox->getMax() - boundingbox->getMin();
		Vec3f side = cell_size * 0.5;
		Vec3f centerpoint = get_center(x, y, z);
		Vec3f points[] = { Vec3f(1,-1,-1), Vec3f(1,1,-1), Vec3f(-1,1,-1), Vec3f(-1,-1,-1),
			Vec3f(1,-1,1), Vec3f(1,1,1), Vec3f(-1,1,1), Vec3f(-1,-1,1) };
		int index[6][4] = { {0,1,5,4}, {2,3,7,6}, {1,2,6,5}, {3,0,4,7}, {4,5,6,7}, {3,2,1,0} };
		vector<Vec3f> result;
		for (int i = 0; i < 4; i++)
		{
			Vec3f t = points[index[type][i]];
			t = t * side + centerpoint;
			result.push_back(t);
		}
		return result;
	}


	void drawface(int x, int y, int z, int type)
	{
		Vec3f nor = getNormal(type);
		glNormal3f(nor.x(), nor.y(), nor.z());
		vector<Vec3f> points = getFace(x, y, z, type);
		for (auto t : points)
		{
			glVertex3f(t.x(), t.y(), t.z());
		}
	}

	void paint()
	{
		//this->material->glSetMaterial();
		// TODO delete useless face
		for (int i = 0; i < nx; i++)
		{
			for (int j = 0; j < ny; j++)
			{
				for (int k = 0; k < nz; k++)
				{
					int size = get_record(i, j, k)->size();
					if (size > 0)
					{
						size = min(size, 10);
						color_collection[size - 1]->glSetMaterial();
						glBegin(GL_QUADS);
						for (int t = 0; t < 6; t++)
						{
							drawface(i, j, k, t);
						}
						glEnd();
					}
				}
			}
		}
	}

	void initializeRayMarch(MarchingInfo& mi, const Ray& r, float tmin) const
	{
		Vec3f dir = r.getDirection();
		dir += Vec3f(dir.x() == 0 ? EPSILON : 0, dir.y() == 0 ? EPSILON : 0, dir.y() == 0 ? EPSILON : 0);
		Vec3f tmin_v = (boundingbox->getMin() - r.getOrigin()) / dir;
		Vec3f tmax_v = (boundingbox->getMax() - r.getOrigin()) / dir;
		float min_intersection = max(min(tmax_v.x(), tmin_v.x()), max(min(tmax_v.y(), tmin_v.y()), min(tmax_v.z(), tmin_v.z())));
		float max_intersection = min(max(tmax_v.x(), tmin_v.x()), min(max(tmax_v.y(), tmin_v.y()), max(tmax_v.z(), tmin_v.z())));
		mi.no_intersection = min_intersection > max_intersection || max_intersection < tmin;
		if (mi.no_intersection)
		{
			return;
		}
		// ray's incremental direction
		float sign_x, sign_y, sign_z;
		sign_x = r.getDirection().x() > 0 ? 1 : -1;
		sign_y = r.getDirection().y() > 0 ? 1 : -1;
		sign_z = r.getDirection().z() > 0 ? 1 : -1;
		if (r.getDirection().x() == 0)
			sign_x = 0;
		if (r.getDirection().y() == 0)
			sign_y = 0;
		if (r.getDirection().z() == 0)
			sign_z = 0;
		mi.direction_sign = Vec3f(sign_x, sign_y, sign_z);
		mi.dt = r.getDirection() / cell_size;
		mi.dt.Normalize();
		//ensure mi.dt.x,y,z !=0 used in finding next cell;
		if (mi.dt.x() == 0)
		{
			mi.dt += Vec3f(0.00001, 0, 0);
		}
		if (mi.dt.y() == 0)
		{
			mi.dt += Vec3f(0, 0.00001, 0);
		}
		if (mi.dt.z() == 0)
		{
			mi.dt += Vec3f(0, 0, 0.00001);
		}
		// inital_point
		mi.tmin = max(tmin, min_intersection);
		Vec3f inital_point = r.pointAtParameter(mi.tmin);
		Vec3f pos = (inital_point - boundingbox->getMin()) / cell_size;
		//cout << inital_point - boundingbox->getMin() << endl;
		//current position of cell
		mi.pos = Vec3f(floor(pos.x()), floor(pos.y()), floor(pos.z()));
		mi.update_index();
		// normal direction
		// note that the normal direction should opposite to ray's intersect point
		mi.normal = Vec3f(0, 0, 0);
		if (min(tmax_v.x(), tmin_v.x()) + EPSILON > min_intersection)
			mi.normal += Vec3f(-sign_x, 0, 0);
		else if (min(tmax_v.y(), tmin_v.y()) + EPSILON > min_intersection)
			mi.normal += Vec3f(0, -sign_y, 0);
		else
			mi.normal += Vec3f(0, 0, -sign_z);
		// I think it's the most tricky part
		// Maybe it's difficult at first to understand what's fucking 0.5 * normal + vec3f(0.5) do
		// It's just calculate the distance to next cell
		// You can just try simple cases to understand it, note that mi.dt is directional, which means it may be negative
		mi.error_term = (mi.pos - pos + 0.5 * mi.direction_sign + Vec3f(0.5, 0.5, 0.5)) / mi.dt;
	}

	bool intersect(const Ray& r, Hit& h, float tmin)
	{
		bool mark = false;
		MarchingInfo mi;
		initializeRayMarch(mi, r, tmin);
		if (mi.no_intersection)
			return false;
		int out = 1;
		//There are one and only one special case that we can't handle the cell
		//It's when the ray hit at the upper bound of cell(either x, y or z)
		//In this case the algorithm think we are out of range
		//But as the ray increase, we'll enter the grid
		//So we need an out to give it a chance 
		while (true)
		{
			auto object_list = get_record(mi.cur_x, mi.cur_y, mi.cur_z);
			if (!object_list)
			{
				if (!out)
				{
					break;
				}
				else
					out--;
			}
			else
			{
				for (int i = 0; i < 6; i++)
				{
					auto faces = getFace(mi.cur_x, mi.cur_y, mi.cur_z, i);
					auto normal = getNormal(i);
					RayTree::AddHitCellFace(faces[0], faces[1], faces[2], faces[3], normal, color_collection[min(MAX_COLOR_SIZE - 1, object_list->size())]);
				}
				//draw enter face
				int index = mi.normal.Dot3(Vec3f(1, 3, 5));
				if (index < 0) index = -index;
				else index -= 1;
				index = max(min(index, 5), 0);
				auto faces = getFace(mi.cur_x, mi.cur_y, mi.cur_z, index);
				RayTree::AddEnteredFace(faces[0], faces[1], faces[2], faces[3], mi.normal, color_collection[min(MAX_COLOR_SIZE - 1, object_list->size())]);
				//if the cell is not empty
				if (object_list->size() > 0)
				{
					if (!mark)
						h.set(200, color_collection[min(MAX_COLOR_SIZE - 1, object_list->size() - 1)], mi.normal, r);
					mark = true;
					return true;
				}

			}
			mi.next_cell();
		}
		return mark;
	}
	//judge if it's intersect with a plane, which is stored in a vector
	bool check_infinite_object(const Ray& r, Hit& h, float tmin)
	{
		bool result = false;
		for (auto& object : infinite_object_list)
		{
			Hit tmp;
			if (object->intersect(r, tmp, tmin))
			{
				if (!result || h.getT() > tmp.getT())
					h = tmp;
				result = true;
			}
		}
		return result;
	}

	bool inside_grid(MarchingInfo mi, Vec3f point)
	{
		Vec3f t = Vec3f(mi.cur_x, mi.cur_y, mi.cur_z);
		Vec3f minimum = boundingbox->getMin() + cell_size * t;
		Vec3f maximum = boundingbox->getMin() + cell_size * (t + Vec3f(1, 1, 1));
		if (point.x() >= minimum.x() && point.y() >= minimum.y() && point.z() >= minimum.z() &&
			point.x() <= maximum.x() && point.y() <= maximum.y() && point.z() <= maximum.z())
		{
			return true;
		}
		return false;
	}

	bool intersect_real(const Ray& r, Hit& h, float tmin)
	{
		MarchingInfo mi;
		initializeRayMarch(mi, r, tmin);
		Hit plane_hit;
		bool plane_result = check_infinite_object(r, plane_hit, tmin);
		bool result = false; //if we obtain a interection point now
		// special cases when plane is front of boundingbox or don't intersect with boundingbpox 
		if (mi.no_intersection || (plane_result && mi.tmin > plane_hit.getT()))
		{
			h = plane_hit;
			return plane_result;
		}

		int out = 1;
		unordered_set<Object3D*> set;
		//So we need an out to give it a chance 
		while (true)
		{
			auto object_list = get_record(mi.cur_x, mi.cur_y, mi.cur_z);
			if (!object_list)
			{
				if (!out)
					break;
				else
					out--;
			}
			else
			{
				for (auto& object : *object_list)
					if (set.find(object) == set.end())
					{
						Hit tmp;
						if (object->intersect(r, tmp, tmin))
						{
							if (!result || tmp.getT() < h.getT())
								h = tmp;
							result = true;
						}
						set.insert(object);
					}
				if (result && inside_grid(mi, r.pointAtParameter(h.getT())))
				{
					return true;
				}
			}
			//cout << mi.cur_x << mi.cur_y << mi.cur_z << endl;
			mi.next_cell();
		}
		return false;
	}
};