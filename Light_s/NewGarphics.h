#ifndef _NEW_GRAPHICS_H_
#define _NEW_GRAPHICS_H_

#include "Graphics.h"

class NewGarphics : public Graphics
{
	// Variables
private:
	//std::vector<fPoint2D> points;
	//float angle;

	//mat3x3 mXtoRight;
	//mat3x3 mXtoLeft;
	//mat3x3 mYtoUp;
	//mat3x3 mYtoDown;

	// 3D
	mesh cube;

	float fThetaX, fThetaY, fThetaZ;
	float scale;						// Для масштаба
	float _x, _y;						// Для сдвигов

	mat4x4 matProj;		// Matrix that converts from view space to screen space



	// Overrided methods
private:
	virtual void OnUserCreate() override;
	virtual void OnUserUpdate(float fElapsedTime) override;
};

#endif // !_NEW_GRAPHICS_H_
