#pragma once
#include"sample.h"
class Sampler
{
protected:
	int sample_per_pixel;
public:
	Sampler(int n)
	{
		this->sample_per_pixel = n;
	}

	virtual Vec2f getSamplePosition(int n) = 0;
};

class RandomSampler : public Sampler
{
public:
	RandomSampler(int n) : Sampler(n)
	{

	}

	virtual Vec2f getSamplePosition(int n)
	{
		return Vec2f(rand() % 101 * 1.0 / 100, rand() % 101 * 1.0 / 100);
	}
};

class UniformSampler : public Sampler
{
protected:
	int line_size;
public:
	UniformSampler(int n) : Sampler(n)
	{
		line_size = round(sqrt(n));
	}

	virtual Vec2f getSamplePosition(int n)
	{
		float x_bias = n % line_size;
		float y_bias = n / line_size;
		return Vec2f(1. / line_size * (x_bias + 0.5), 1. / line_size * (y_bias + 0.5));
	}

	float get_len()
	{
		return this->line_size;
	}
};

class JitteredSampler : public Sampler
{
	UniformSampler uniform_sampler;
	RandomSampler random_sampler;
public:
	JitteredSampler(int n) : Sampler(n), uniform_sampler(UniformSampler(n)), random_sampler(RandomSampler(n))
	{

	}

	virtual Vec2f getSamplePosition(int n)
	{
		Vec2f v1 = uniform_sampler.getSamplePosition(n);
		Vec2f v2 = random_sampler.getSamplePosition(n);
		float x_offset = v1.x() + (-0.5 + v2.x()) / uniform_sampler.get_len();
		float y_offset = v1.y() + (-0.5 + v2.y()) / uniform_sampler.get_len();
		return Vec2f(x_offset, y_offset);
	}
};
