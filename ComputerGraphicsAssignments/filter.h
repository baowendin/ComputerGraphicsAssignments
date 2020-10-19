#pragma once
#include<math.h>
#include<algorithm>
#include"film.h"
using namespace std;
class Filter
{
public:
	Filter() {}
	virtual float getWeight(float x, float y) = 0;
	virtual int getSupportRadius() = 0;

	Vec3f getColor(int x, int y, Film* film)
	{
		float weight = 0;
		Vec3f color = Vec3f();
		int r = getSupportRadius();
		int samples = film->getNumSamples();
		for (int i = -r; i <= r; i++)
		{
			for (int j = -r; j <= r; j++)
			{
				int new_x = x + i;
				int new_y = y + j;
				if (new_x < 0 || new_y < 0 || new_x >= film->getWidth() || new_y >= film->getHeight())
					continue;
				for (int t = 0; t < samples; t++)
				{
					Sample temp = film->getSample(new_x, new_y, t);
					float c = getWeight(new_x + temp.getPosition().x() - x, new_y + temp.getPosition().y() - y);
					color += temp.getColor() * c;
					weight += c;
				}
			}
		}
		color /= weight;
		return color;
	}
};

class BoxFilter : public Filter
{
protected:
	float radius;
public:
	BoxFilter(float radius)
	{
		this->radius = radius;
	}

	virtual float getWeight(float x, float y)
	{
		return (x >= -radius && x <= radius && y >= -radius && y <= radius) ? 1 : 0;
	}

	virtual int getSupportRadius() { return ceil(this->radius); }
};

class TentFilter : public Filter
{
protected:
	float radius;
public:
	TentFilter(float radius)
	{
		this->radius = radius;
	}

	virtual float getWeight(float x, float y)
	{
		return max(1.0 - sqrt(x * x + y * y) / radius, 0.);
	}

	virtual int getSupportRadius() { return ceil(this->radius); }
};

class GaussianFilter : public Filter
{
protected:
	float sigma;
public:
	GaussianFilter(float sigma)
	{
		this->sigma = sigma;
	}

	virtual float getWeight(float x, float y)
	{
		float d_2 = x * x + y * y;
		return exp(-d_2 / (2 * sigma * sigma));
	}

	virtual int getSupportRadius() { return ceil(2 * this->sigma); }
};

