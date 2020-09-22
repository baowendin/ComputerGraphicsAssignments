#ifndef _MATERIAL_H_
#define _MATERIAL_H_
#include <windows.h>  //Windows Header
#include <GL\gl.h>   //OpenGL Header
#include <GL\glu.h>   //GLu32 Header
#include "vectors.h"
#include "hit.h"
#include "glCanvas.h"
#ifdef SPECULAR_FIX
// OPTIONAL:  global variable allows (hacky) communication 
// with glCanvas::display
extern int SPECULAR_FIX_WHICH_PASS;
#endif


// ====================================================================
// ====================================================================

// You will extend this class in later assignments

class Material {

public:

	// CONSTRUCTORS & DESTRUCTOR
	Material(const Vec3f& d_color) { diffuseColor = d_color; }
	virtual ~Material() {}

	// ACCESSORS
	virtual Vec3f getDiffuseColor() const { return diffuseColor; }
	virtual Vec3f Shade (const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const = 0;
	virtual void glSetMaterial(void) const = 0;

protected:

	// REPRESENTATION
	Vec3f diffuseColor;

};

// ====================================================================
// ====================================================================
class PhongMaterial : public Material
{
    Vec3f specularColor;
    float exponent;
public:
	PhongMaterial(const Vec3f& diffuseColor,
		const Vec3f& specularColor, float exponent) : Material(diffuseColor)
	{
        this->exponent = exponent;
        this->specularColor = specularColor;
	}

    Vec3f getSpecularColor() const { return diffuseColor; }

    virtual Vec3f Shade(const Ray& ray, const Hit& hit, const Vec3f& dirToLight, const Vec3f& lightColor) const
    {
        return Vec3f(0, 0, 0);
    }

    void glSetMaterial(void) const {

        GLfloat one[4] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat zero[4] = { 0.0, 0.0, 0.0, 0.0 };
        GLfloat specular[4] = {
          getSpecularColor().r(),
          getSpecularColor().g(),
          getSpecularColor().b(),
          1.0 };
        GLfloat diffuse[4] = {
          getDiffuseColor().r(),
          getDiffuseColor().g(),
          getDiffuseColor().b(),
          1.0 };

        // NOTE: GL uses the Blinn Torrance version of Phong...      
        float glexponent = exponent;
        if (glexponent < 0) glexponent = 0;
        if (glexponent > 128) glexponent = 128;

#if !SPECULAR_FIX 

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

#else

        // OPTIONAL: 3 pass rendering to fix the specular highlight 
        // artifact for small specular exponents (wide specular lobe)

        if (SPECULAR_FIX_WHICH_PASS == 0) {
            // First pass, draw only the specular highlights
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, zero);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &glexponent);

        }
        else if (SPECULAR_FIX_WHICH_PASS == 1) {
            // Second pass, compute normal dot light 
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, one);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, zero);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
        }
        else {
            // Third pass, add ambient & diffuse terms
            assert(SPECULAR_FIX_WHICH_PASS == 2);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, zero);
        }

#endif
    }

};
#endif