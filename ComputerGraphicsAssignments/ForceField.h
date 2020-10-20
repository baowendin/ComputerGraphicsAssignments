#pragma once
#include "vectors.h"
class ForceField
{
public:
	virtual Vec3f getAcceleration(const Vec3f& position, float mass, float t) const = 0;
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