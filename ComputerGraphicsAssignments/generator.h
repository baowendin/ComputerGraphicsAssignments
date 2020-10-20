#pragma once
#include "vectors.h"
#include "particle.h"
#include "random.h"
class Generator
{
protected:
    Vec3f color, dead_color;
    float color_randomness;
    float lifespan, lifespan_randomness;
    int desired_num_particles;
    float mass, mass_randomness;
public:
    void SetColors(Vec3f color, Vec3f dead_color, float color_randomness)
    {
        this->color = color;
        this->dead_color = dead_color;
        this->color_randomness = color_randomness;
    }

    void SetLifespan(float lifespan, float lifespan_randomness, int desired_num_particles)
    {
        this->lifespan = lifespan;
        this->lifespan_randomness = lifespan_randomness;
        this->desired_num_particles = desired_num_particles;
    }

    void SetMass(float mass, float mass_randomness)
    {
        this->mass = mass;
        this->mass_randomness = mass_randomness;
    }

    // on each timestep, create some particles
    virtual int numNewParticles(float current_time, float dt) const = 0;
    virtual Particle* Generate(float current_time, int i) = 0;

    // for the gui
    void Paint() const {}
    void Restart() {}
};

static Vec3f get_random_v()
{
    static Random random;
    return Vec3f(random.next(), random.next(), random.next());
}

static float get_random_f()
{
    static Random random;
    return random.next();
}

class HoseGenerator : public Generator
{
protected:
    Vec3f position;
    float position_randomness;
    Vec3f velocity;
    float velocity_randomness;
public:
    HoseGenerator(Vec3f position, float position_randomness, Vec3f velocity, float velocity_randomness)
    {
        this->position = position;
        this->position_randomness = position_randomness;
        this->velocity = velocity;
        this->velocity_randomness = velocity_randomness;
    }

    virtual int numNewParticles(float current_time, float dt) const
    {
        return desired_num_particles * (1. / lifespan) * dt;
    }

    virtual Particle* Generate(float current_time, int i)
    {     
        Vec3f p = position + get_random_v() * position_randomness;
        Vec3f v = velocity + get_random_v() * velocity_randomness;
        Vec3f c = color + get_random_v() * color_randomness;
        Vec3f dead_c = dead_color;
        float m = mass + mass_randomness * get_random_f();
        float ls = lifespan + lifespan_randomness * get_random_f();
        return new Particle(p, v, c, dead_c, m, ls);
    }
};

class RingGenerator : public Generator
{
protected:
    float position_randomness;
    Vec3f velocity;
    float velocity_randomness;
public:
    RingGenerator(float position_randomness, Vec3f velocity, float velocity_randomness)
    {
        this->position_randomness = position_randomness;
        this->velocity = velocity;
        this->velocity_randomness = velocity_randomness;
    }

    virtual int numNewParticles(float current_time, float dt) const
    {
        return dt * desired_num_particles / lifespan* current_time * 0.1;
    }

    virtual Particle* Generate(float current_time, int i)
    {
        Vec3f dir = Vec3f(get_random_f(), 0, get_random_f());
        dir.Normalize();

        Vec3f p = dir * current_time * 0.25;
        Vec3f v = velocity + get_random_v() * velocity_randomness;
        Vec3f c = color + get_random_v() * color_randomness;
        Vec3f dead_c = dead_color;
        float m = mass + mass_randomness * get_random_f();
        float ls = lifespan + lifespan_randomness * get_random_f();
        return new Particle(p, v, c, dead_c, m, ls);
    }

};