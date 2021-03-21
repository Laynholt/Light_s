#include "NewGarphics.h"

void NewGarphics::OnUserCreate()
{
	points.resize(4);
	//points = { {70, 50}, {75, 45}, {85, 45}, {80, 50} };
	points = { {70, 50}, {20, 15}, {130, 15}, {80, 50} };

	angle = 0.0f;

	mXtoRight.m[0][0]	= 1.0f; mXtoRight.m[1][1]	= 1.0f; mXtoRight.m[2][2]	= 1.0f;
	mXtoLeft.m[0][0]	= 1.0f; mXtoLeft.m[1][1]	= 1.0f; mXtoLeft.m[2][2]	= 1.0f;
	mYtoUp.m[0][0]		= 1.0f; mYtoUp.m[1][1]		= 1.0f; mYtoUp.m[2][2]		= 1.0f;
	mYtoDown.m[0][0]	= 1.0f; mYtoDown.m[1][1]	= 1.0f; mYtoDown.m[2][2]	= 1.0f;
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
		mXtoRight.m[1][0] = 100.0f * fElapsedTime;
		MoveTo2D(points, mXtoRight);
	}
	if (GetKey('Q').bHeld)
	{
		mXtoLeft.m[1][0] = -100.0f * fElapsedTime;
		MoveTo2D(points, mXtoLeft);
	}
	if (GetKey('R').bHeld)
	{
		mYtoUp.m[0][1] = -100.0f * fElapsedTime;
		MoveTo2D(points, mYtoUp);
	}
	if (GetKey('F').bHeld)
	{
		mYtoDown.m[0][1] = 100.0f * fElapsedTime;
		MoveTo2D(points, mYtoDown);
	}
	

	//DrawLineBresenham(points[0].x, points[0].y, points[1].x, points[1].y, ' ', BG_WHITE);
	DrawPolygons(points, ' ', BG_WHITE);
	//ShadingPolygonsScanLine(points);
	ShadingPolygonsFloodFill(points);
}


//void NewGarphics::OnUserCreate()
//{
//	cube.tris =
//	{
//			// SOUTH
//			{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
//			{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },
//
//			// EAST                                                      
//			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
//			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },
//
//			// NORTH                                                     
//			{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
//			{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },
//
//			// WEST                                                      
//			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
//			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },
//
//			// TOP                                                       
//			{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
//			{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },
//
//			// BOTTOM                                                    
//			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
//			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
//	};
//
//	matProj = Matrix_MakeProjection(90.0f, (float)GetConsoleHeight() / (float)GetConsoleWidth(), 0.1f, 1000.0f);
//
//	fYaw = fTheta = 0.0f;
//}
//
//void NewGarphics::OnUserUpdate(float fElapsedTime)
//{
//	// Clear our console
//	ClearConsole(0, 0, GetConsoleWidth(), GetConsoleHeight());
//
//	mat4x4 matRotX, matRotY, matRotZ;
//
//	fTheta += 1.0f * fElapsedTime;
//
//	matRotX = Matrix_MakeRotationX(fTheta * 0.5f);
//	matRotY = Matrix_MakeRotationX(fTheta * 0.5f);
//	matRotZ = Matrix_MakeRotationX(fTheta);
//
//	for (auto& tri : cube.tris)
//	{
//
//	}
//}