#ifndef _NEW_GRAPHICS_H_
#define _NEW_GRAPHICS_H_

#include "Graphics.h"

class NewGarphics : public Graphics
{
	// Variables
private:
	std::vector<fPoint> points;
	float angle;

	// Overrided methods
private:
	virtual void OnUserCreate() override;
	virtual void OnUserUpdate(float fElapsedTime) override;
};

#endif // !_NEW_GRAPHICS_H_
