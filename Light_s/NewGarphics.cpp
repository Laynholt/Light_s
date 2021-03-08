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
}

void NewGarphics::OnUserUpdate(float fElapsedTime)
{
	// Clear our console
	Fill(0, 0, GetConsoleWidth(), GetConsoleHeight(), PIXEL_SOLID, FG_BLACK);

	//DrawLineBresenham(points[0].x, points[0].y, points[1].x, points[1].y, ' ', BG_WHITE);
	DrawPolygons(points, ' ', BG_WHITE);
}
