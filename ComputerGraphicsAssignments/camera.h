#ifndef _CAMERA_H
#define _CAMERA_H
#include "vectors.h"
#include "ray.h"
class Camera
{
public:
	virtual Ray generateRay(Vec2f point) = 0;
	virtual float getTMin() const = 0;
protected:
	float tmin;
};
#endif // !_CAMERA_H
