#pragma once
#include "spline.h"
#include<Windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include "matrix.h"
#include<vector>
using namespace std;
class Curve : public Spline
{
protected:
	vector<Vec3f> vertices;
	int num;
public:
	Curve(int num)
	{
		this->num = num;
		this->vertices.resize(num);
	}

	void set(int index, Vec3f vertex)
	{
		vertices[index] = vertex;
	}

	virtual vector<Vec3f> calculate_points(int tessellation)
	{
		return vector<Vec3f>();
	}

	virtual int getNumVertices() { return num; }

	Vec3f getVertex(int i) { return vertices[i]; };

	virtual void moveControlPoint(int selectedPoint, float x, float y) { vertices[selectedPoint].Set(x, y, 0); }

	virtual void addControlPoint(int selectedPoint, float x, float y)
	{
		if (selectedPoint > num)
			return;
		vertices.insert(vertices.begin() + selectedPoint, Vec3f(x, y, 0));
		num++;
	}

	virtual void deleteControlPoint(int selectedPoint)
	{
		if (num == 4 || selectedPoint < 0 || selectedPoint >= num)
			return;
		vertices.erase(vertices.begin() + selectedPoint);
		num--;
	}

	virtual void Paint(ArgParser* args)
	{
		//Convex Hull
		glColor3f(17. / 255, 50. / 255, 133. / 255); // KONJYO
		glLineWidth(1);
		glBegin(GL_LINES);
		for (int i = 1; i < num; i++)
		{
			glVertex3f(vertices[i - 1].x(), vertices[i - 1].y(), vertices[i - 1].z());
			glVertex3f(vertices[i].x(), vertices[i].y(), vertices[i].z());
		}
		glEnd();

		//Point
		glColor3f(1, 0, 0); // Red
		glPointSize(2);
		glBegin(GL_POINTS);
		for (int i = 0; i < num; i++)
		{
			glVertex3f(vertices[i].x(), vertices[i].y(), vertices[i].z());
		}
		glEnd();

		//Real Cruve
		glColor3f(0.f, 1.f, 0.f);
		glLineWidth(1);
		glBegin(GL_LINES);
		vector<Vec3f> cruve_vertices = calculate_points(args->curve_tessellation);
		cout << vertices.size() << endl;
		for (int i = 1; i < cruve_vertices.size(); i++)
		{
			glVertex3f(cruve_vertices[i - 1].x(), cruve_vertices[i - 1].y(), cruve_vertices[i - 1].z());
			glVertex3f(cruve_vertices[i].x(), cruve_vertices[i].y(), cruve_vertices[i].z());
		}
		glEnd();
	}
};

class BezierCurve : public Curve
{
public:
	static Vec3f calculate_point(Vec3f v1, Vec3f v2, Vec3f v3, Vec3f v4, float t)
	{
		static const float f[] = { -1, 3, -3, 1, 3, -6, 3, 0, -3, 3, 0, 0, 1, 0, 0, 0 };
		static const Matrix m(f);
		Vec4f TVector(t * t * t, t * t, t, 1);
		m.Transform(TVector);
		return v1 * TVector.x() + v2 * TVector.y() + v3 * TVector.z() + v4 * TVector.w();
	}

	Vec3f calculate_point(int l, float t)
	{
		static const float f[] = { -1, 3, -3, 1, 3, -6, 3, 0, -3, 3, 0, 0, 1, 0, 0, 0 };
		static const Matrix m(f);
		Vec4f TVector(t * t * t, t * t, t, 1);
		m.Transform(TVector);
		return vertices[l] * TVector.x() + vertices[l + 1] * TVector.y() + vertices[l + 2] * TVector.z() + vertices[l + 3] * TVector.w();
	}

	BezierCurve(int l) : Curve(l) {}

	virtual vector<Vec3f> calculate_points(int tessellation)
	{
		vector<Vec3f> result;
		float unit_len = 1.0 / tessellation;
		//every four points is a unit to represenet a bezier curve
		for (int i = 0; i < num - 3; i += 3)
		{
			for (int j = 0; j <= tessellation; j++)
			{
				result.push_back(calculate_point(i, j * unit_len));
			}
		}
		return result;
	}
};

class BSplineCurve : public Curve
{
public:
	Vec3f calculate_point(int l, float t)
	{
		static const float f[] = { -1, 3, -3, 1, 3, -6, 0, 4, -3, 3, 3, 1, 1, 0, 0, 0 };
		static const Matrix m(f);
		Vec4f TVector(t * t * t, t * t, t, 1);
		TVector *= (1.f / 6);
		m.Transform(TVector);
		return vertices[l] * TVector.x() + vertices[l + 1] * TVector.y() + vertices[l + 2] * TVector.z() + vertices[l + 3] * TVector.w();
	}

	BSplineCurve(int l) : Curve(l) {}

	vector<Vec3f> calculate_points(int tessellation)
	{
		vector<Vec3f> result;
		float unit_len = 1. / tessellation;

		for (int i = 0; i < num - 3; i++)
		{
			for (int j = 0; j <= tessellation; j++)
			{
				Vec3f m = calculate_point(i, unit_len * j);
				result.push_back(m);
			}
		}
		return result;
	}
};
