#pragma once
#include "vectors.h"
class ForceField
{
public:
	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t) = 0;
};

class GravityForceField : public ForceField
{
protected:
	Vec3f gravity;
public:
	GravityForceField(Vec3f gravity)
	{
		this->gravity = gravity;
	}

	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t)
	{
		return gravity;
	}
};

class ConstantForceField : public ForceField
{
protected:
	Vec3f force;
public:
	ConstantForceField(Vec3f force)
	{
		this->force = force;
	}

	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t)
	{
		return force * (1.0 / mass);
	}

};

class RadialForceField : public ForceField
{
protected:
	float magnitude;
public:
	RadialForceField(float magnitude)
	{
		this->magnitude = magnitude;
	}

	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t)
	{
		return position * magnitude * -1;
	}
};

class VerticalForceField : public ForceField
{
protected:
	float magnitude;
public:
	VerticalForceField(float magnitude)
	{
		this->magnitude = magnitude;
	}

	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t)
	{
		return Vec3f(0, position.y() * magnitude * -1, 0);
	}
};