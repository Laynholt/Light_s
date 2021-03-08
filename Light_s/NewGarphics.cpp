#include "NewGarphics.h"

void NewGarphics::OnUserCreate()
{
	points.resize(4);

	points[0].x = 30;
	points[0].y = 50;
	points[1].x = 120;
	points[1].y = 10;
	points[2].x = 140;
	points[2].y = 30;
	points[3].x = 70;
	points[3].y = 60;

	angle = 0.0f;
}

void NewGarphics::OnUserUpdate(float fElapsedTime)
{
	// Clear our console
	Fill(0, 0, GetConsoleWidth(), GetConsoleHeight(), PIXEL_SOLID, FG_BLACK);

	if (GetKey('W').bHeld) { ScalingPolygons(points, 1.01f); }
	if (GetKey('S').bHeld) { ScalingPolygons(points, 0.99f); }
	if (GetKey('A').bHeld) { angle = -0.9f; RotatePolygons(points, angle); }
	if (GetKey('D').bHeld) { angle = 0.9f; RotatePolygons(points, angle); }
	if (GetKey('R').bHeld)
	{
		for (int16_t i = 0; i < points.size(); i++)
		{
			points[i].x += 100.0f * fElapsedTime;
		}
	}
	if (GetKey('Q').bHeld)
	{
		for (int16_t i = 0; i < points.size(); i++)
		{
			points[i].x -= 100.0f * fElapsedTime;
		}
	}

	//DrawLineBresenham(points[0].x, points[0].y, points[1].x, points[1].y, ' ', BG_WHITE);
	DrawPolygons(points, ' ', BG_WHITE);
	ShadingPolygons(points);
}
