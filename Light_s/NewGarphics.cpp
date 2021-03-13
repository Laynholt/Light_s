#include "NewGarphics.h"

void NewGarphics::OnUserCreate()
{
	points.resize(4);
	//points = { {70, 50}, {75, 45}, {85, 45}, {80, 50} };
	points = { {70, 50}, {20, 15}, {130, 15}, {80, 50} };

	angle = 0.0f;
}

void NewGarphics::OnUserUpdate(float fElapsedTime)
{
	// Clear our console
	ClearConsole(0, 0, GetConsoleWidth(), GetConsoleHeight());

	if (GetKey('W').bHeld) { ScalingPolygons(points, 1.01f); }
	if (GetKey('S').bHeld) { ScalingPolygons(points, 0.99f); }
	if (GetKey('A').bHeld) { angle = -0.9f; RotatePolygons(points, angle); }
	if (GetKey('D').bHeld) { angle = 0.9f; RotatePolygons(points, angle); }
	if (GetKey('E').bHeld)
	{
		for (auto& point : points)
		{
			point.x += 100.0f * fElapsedTime;
		}
	}
	if (GetKey('Q').bHeld)
	{
		for (auto& point : points)
		{
			point.x -= 100.0f * fElapsedTime;
		}
	}
	if (GetKey('F').bHeld)
	{
		for (auto& point : points)
		{
			point.y += 100.0f * fElapsedTime;
		}
	}
	if (GetKey('R').bHeld)
	{
		for (auto& point : points)
		{
			point.y -= 100.0f * fElapsedTime;
		}
	}

	//DrawLineBresenham(points[0].x, points[0].y, points[1].x, points[1].y, ' ', BG_WHITE);
	DrawPolygons(points, ' ', BG_WHITE);
	//ShadingPolygonsScanLine(points);
	ShadingPolygonsFloodFill(points);
}
