#include "NewGarphics.h"

//void NewGarphics::OnUserCreate()
//{
//	points.resize(4);
//	//points = { {70, 50}, {75, 45}, {85, 45}, {80, 50} };
//	points = { {70, 50}, {20, 15}, {130, 15}, {80, 50} };
//
//	angle = 0.0f;
//
//	mXtoRight.m[0][0]	= 1.0f; mXtoRight.m[1][1]	= 1.0f; mXtoRight.m[2][2]	= 1.0f;
//	mXtoLeft.m[0][0]	= 1.0f; mXtoLeft.m[1][1]	= 1.0f; mXtoLeft.m[2][2]	= 1.0f;
//	mYtoUp.m[0][0]		= 1.0f; mYtoUp.m[1][1]		= 1.0f; mYtoUp.m[2][2]		= 1.0f;
//	mYtoDown.m[0][0]	= 1.0f; mYtoDown.m[1][1]	= 1.0f; mYtoDown.m[2][2]	= 1.0f;
//}
//
//void NewGarphics::OnUserUpdate(float fElapsedTime)
//{
//	// Clear our console
//	ClearConsole(0, 0, GetConsoleWidth(), GetConsoleHeight());
//
//	if (GetKey('W').bHeld) { ScalingPolygons(points, 1.01f); }
//	if (GetKey('S').bHeld) { ScalingPolygons(points, 0.99f); }
//	if (GetKey('A').bHeld) { angle = -0.9f; RotatePolygons(points, angle); }
//	if (GetKey('D').bHeld) { angle = 0.9f; RotatePolygons(points, angle); }
//	if (GetKey('E').bHeld)
//	{
//		mXtoRight.m[1][0] = 100.0f * fElapsedTime;
//		MoveTo2D(points, mXtoRight);
//	}
//	if (GetKey('Q').bHeld)
//	{
//		mXtoLeft.m[1][0] = -100.0f * fElapsedTime;
//		MoveTo2D(points, mXtoLeft);
//	}
//	if (GetKey('R').bHeld)
//	{
//		mYtoUp.m[0][1] = -100.0f * fElapsedTime;
//		MoveTo2D(points, mYtoUp);
//	}
//	if (GetKey('F').bHeld)
//	{
//		mYtoDown.m[0][1] = 100.0f * fElapsedTime;
//		MoveTo2D(points, mYtoDown);
//	}
//	
//
//	//DrawLineBresenham(points[0].x, points[0].y, points[1].x, points[1].y, ' ', BG_WHITE);
//	DrawPolygons(points, ' ', BG_WHITE);
//	//ShadingPolygonsScanLine(points);
//	ShadingPolygonsFloodFill(points);
//}


void NewGarphics::OnUserCreate()
{
	cube.tris =
	{
			// SOUTH
			{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

			// EAST                                                      
			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

			// NORTH                                                     
			{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

			// WEST                                                      
			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

			// TOP                                                       
			{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

			// BOTTOM                                                    
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
	};

	matProj = Matrix_MakeProjection(90.0f, (float)GetConsoleHeight() / (float)GetConsoleWidth(), 0.1f, 1000.0f);



	fThetaX = fThetaY = fThetaZ = 0.0f;
	scale = 1.0f;							
	_x = 0.65f, _y = 0.5f; 
}

void NewGarphics::OnUserUpdate(float fElapsedTime)
{
	// Clear our console
	ClearConsole(0, 0, GetConsoleWidth(), GetConsoleHeight());

	// Повороты вокруг осей
	if (GetKey(L'W').bHeld)
		fThetaX += 8.0f * fElapsedTime;
	if (GetKey(L'S').bHeld)
		fThetaX -= 8.0f * fElapsedTime;
	if (GetKey(L'A').bHeld)
		fThetaZ += 8.0f * fElapsedTime;
	if (GetKey(L'D').bHeld)
		fThetaZ -= 8.0f * fElapsedTime;
	if (GetKey(L'Q').bHeld)
		fThetaY += 8.0f * fElapsedTime;
	if (GetKey(L'E').bHeld)
		fThetaY -= 8.0f * fElapsedTime;

	// Масштабирование
	if (GetKey(L'Z').bHeld)		// Увеличение
		scale = (scale <= 1.5f) ? scale + 0.01f : scale;
	if (GetKey(L'X').bHeld)		// Уменьшение
		scale = (scale >= 0.5f) ? scale - 0.01f : scale;

	// Сдвиги
	if (GetKey(L'R').bHeld)		// Сдвиг вправо
		_x += 0.01f;
	if (GetKey(L'F').bHeld)		// Сдвиг влево
		_x -= 0.01f;
	if (GetKey(L'T').bHeld)		// Сдвиг вверх
		_y -= 0.01f;
	if (GetKey(L'G').bHeld)		// Сдвиг вниз
		_y += 0.01f;


	mat4x4 matRotX, matRotY, matRotZ;
	matRotX = Matrix_MakeRotationX(fThetaX * 0.5f);
	matRotY = Matrix_MakeRotationY(fThetaY * 0.5f);
	matRotZ = Matrix_MakeRotationZ(fThetaZ * 0.5f);

	mat4x4 ScalingMatrix;
	ScalingMatrix = Matrix_MakeScale(scale, scale, scale);

	mat4x4 TranslationMatrix;
	TranslationMatrix = Matrix_MakeTranslation(0.0f, 0.0f, 0.5f);

	mat4x4 WorldMatrix;
	WorldMatrix = Matrix_MakeIdentity();
	WorldMatrix = matRotY * matRotX * matRotZ * ScalingMatrix * TranslationMatrix;

	std::vector<fPoint2D> lines(3);
	std::vector<triangle> vecTrianglesToRaster;

	float  t = 0.0f;
	//for (int16_t i = 0; i < 2; i++, t += 1.4f) 
	{
		// Проходимся по всем треугольникам
		for (auto tri : cube.tris)
		{
			triangle triProjected, triTransformed;
			

			triTransformed.p[0] = MultiplyMatrixVector(WorldMatrix, tri.p[0]);
			triTransformed.p[1] = MultiplyMatrixVector(WorldMatrix, tri.p[1]);
			triTransformed.p[2] = MultiplyMatrixVector(WorldMatrix, tri.p[2]);


			// Переводим треугольники из 3D --> 2D
			triProjected.p[0] = MultiplyMatrixVector(matProj, triTransformed.p[0]);
			triProjected.p[1] = MultiplyMatrixVector(matProj, triTransformed.p[1]);
			triProjected.p[2] = MultiplyMatrixVector(matProj, triTransformed.p[2]);

			// Масштабируем под размер консоли
			for (int16_t i = 0; i < 3; i++)
			{
				triProjected.p[i].x += _x + t;
				triProjected.p[i].y += _y;

				triProjected.p[i].x *= 0.3f * (float)GetConsoleWidth();
				triProjected.p[i].y *= 0.3f * (float)GetConsoleHeight();
			}

			vecTrianglesToRaster.push_back(triProjected);
		}

		// Sort triangles from back to front
		std::sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
			{
				float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
				float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
				return z1 < z2;
			});

		for (auto& tri : vecTrianglesToRaster)
		{
			for (int16_t i = 0; i < 3; i++)
			{
				tri.p[i].x = roundf(tri.p[i].x);
				tri.p[i].y = roundf(tri.p[i].y);
			}
		}


		WarnockAlgorithm(vecTrianglesToRaster, 0.0f, iConsoleWidth - 1, 0.0f, iConsoleHeight - 1);

		//for (auto& tri : vecTrianglesToRaster)
		//{
		//	for (int16_t i = 0; i < 3; i++)
		//	{
		//		lines[i].x = tri.p[i].x;
		//		lines[i].y = tri.p[i].y;
		//	}

		//	// Выводим на экран

		//	ShadingPolygonsScanLine(lines);
		//	//DrawPolygons(lines, PIXEL_SOLID, FG_GREY);
		//}

		

		vecTrianglesToRaster.clear();
	}

	

}