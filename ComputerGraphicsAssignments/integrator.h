#pragma once
#include "particle.h"
#include "ForceField.h"
class Integrator
{
public:
	virtual void Update(Particle* particle, ForceField* forcefield, float t, float dt) = 0;

	virtual Vec3f getColor() 
	{
		return Vec3f(109, 46, 91) * (1.0 / 255); // EBIZOME
	}
};

class EulerIntegrator : public Integrator
{
public:
	virtual void Update(Particle* particle, ForceField* forcefield, float t, float dt)
	{
		Vec3f position = particle->getPosition() + particle->getVelocity() * dt;
		Vec3f velocity = particle->getVelocity() + forcefield->getAcceleration(particle->getPosition(), particle->getMass(), t) * dt;

		particle->setPosition(position);
		particle->setVelocity(velocity);
		particle->increaseAge(dt);
	}

	virtual Vec3f getColor()
	{
		return Vec3f(247, 217, 76) * (1.0 / 255); // NANOHANA
	}
};

class MidpointIntegrator : public Integrator
{
public:
	virtual void Update(Particle* particle, ForceField* forcefield, float t, float dt)
	{
		Vec3f p_m = particle->getPosition() + particle->getVelocity() * (dt / 2);
		Vec3f v_m = particle->getVelocity() + forcefield->getAcceleration(particle->getPosition(), particle->getMass(), t) * (dt / 2);
		
		particle->setPosition(particle->getPosition() + v_m * dt);
		particle->setVelocity(particle->getVelocity() + forcefield->getAcceleration(p_m, particle->getMass(), t + dt / 2) * dt);

	}

	virtual Vec3f getColor()
	{
		return Vec3f(181, 202, 160) * (1.0 / 255); // NANOHANA
	}
};