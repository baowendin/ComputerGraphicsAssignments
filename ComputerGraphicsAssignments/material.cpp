#include "material.h"
#include "rayTrace.h"
Vec3f PhongMaterial::Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const
{
	// ray's direction should be normalize since it maybe transform, and it does matter
	Vec3f light_dir = dirToLight;
	light_dir.Normalize();
	Vec3f ray_dir = ray.getDirection();
	ray_dir.Normalize();
	//Diffuse Part
	Vec3f diffuse;
	if (!shade_back)
		diffuse = this->getDiffuseColor() * max(hit.getNormal().Dot3(light_dir), 0);
	else
		diffuse = this->getDiffuseColor() * abs(hit.getNormal().Dot3(light_dir));
	//Specular Part
	//In Blinn-Torrance' s solution, L = (l + v).normalize() dot hit.normal
	//But in our input the ray is from camera to object
	//So l - v is correct
	Vec3f beta = light_dir - ray_dir;
	beta.Normalize();
	Vec3f specular;
	if (!shade_back)
		specular = this->getSpecularColor() * pow(max(beta.Dot3(hit.getNormal()), 0), exponent);
	else
		specular = this->getSpecularColor() * pow(abs(beta.Dot3(hit.getNormal())), exponent);
	//return part
	return (specular + diffuse) * lightColor;
}