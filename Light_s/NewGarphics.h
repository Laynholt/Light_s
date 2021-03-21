#ifndef _NEW_GRAPHICS_H_
#define _NEW_GRAPHICS_H_

#include "Graphics.h"

class NewGarphics : public Graphics
{
	// Variables
private:
	std::vector<fPoint2D> points;
	float angle;

	mat3x3 mXtoRight;
	mat3x3 mXtoLeft;
	mat3x3 mYtoUp;
	mat3x3 mYtoDown;

	// 3D
	mesh cube;

	mat4x4 matProj;		// Matrix that converts from view space to screen space
	fPoint3D vCamera;	// Location of camera in world space
	fPoint3D vLookDir;	// Direction vector along the direction camera points
	float fYaw;			// FPS Camera rotation in XZ plane
	float fTheta;		// Spins World transform


	// Overrided methods
private:
	virtual void OnUserCreate() override;
	virtual void OnUserUpdate(float fElapsedTime) override;
};

#endif // !_NEW_GRAPHICS_H_
