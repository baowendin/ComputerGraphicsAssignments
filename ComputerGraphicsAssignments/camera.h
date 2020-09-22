#ifndef _CAMERA_H
#define _CAMERA_H
#include "vectors.h"
#include "ray.h"
#include <windows.h>  //Windows Header
#include <GL\gl.h>   //OpenGL Header
#include <GL\glu.h>   //GLu32 Header
class Camera
{
public:
	virtual Ray generateRay(Vec2f point) = 0;
	virtual float getTMin() const = 0;
	virtual void glInit(int w, int h) = 0;
	virtual void glPlaceCamera(void) = 0;
	virtual void dollyCamera(float dist) = 0;
	virtual void truckCamera(float dx, float dy) = 0;
	virtual void rotateCamera(float rx, float ry) = 0;
protected:
	float tmin;
};
#endif // !_CAMERA_H
