#include "NewGarphics.h"

void NewGarphics::OnUserCreate()
{
	shapes.resize(2);

	shapes[0].tris =
	{
			// SOUTH
			{ 0.0f, 0.0f, 0.0f,    0.0f, 2.0f, 0.0f,    1.0f, 2.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f,    1.0f, 2.0f, 0.0f,    1.0f, 0.0f, 0.0f },

			// EAST                                                      
			{ 1.0f, 0.0f, 0.0f,    1.0f, 2.0f, 0.0f,    1.0f, 2.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f,    1.0f, 2.0f, 1.0f,    1.0f, 0.0f, 1.0f },

			// NORTH                                                     
			{ 1.0f, 0.0f, 1.0f,    1.0f, 2.0f, 1.0f,    0.0f, 2.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 2.0f, 1.0f,    0.0f, 0.0f, 1.0f },

			// WEST                                                      
			{ 0.0f, 0.0f, 1.0f,    0.0f, 2.0f, 1.0f,    0.0f, 2.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f,    0.0f, 2.0f, 0.0f,    0.0f, 0.0f, 0.0f },

			// TOP                                                       
			{ 0.0f, 2.0f, 0.0f,    0.0f, 2.0f, 1.0f,    1.0f, 2.0f, 1.0f },
			{ 0.0f, 2.0f, 0.0f,    1.0f, 2.0f, 1.0f,    1.0f, 2.0f, 0.0f },

			// BOTTOM                                                    
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
	};

	shapes[1].tris =
	{
		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 2.0f, 0.0f,    1.0f, 2.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 2.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 2.0f, 0.0f,    1.0f, 2.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 2.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 0.0f,    0.0f, 2.0f, 0.0f,    1.0f, 2.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 1.0f,    1.0f, 2.0f, 1.0f },

		// TOP                                                       
		{ 0.0f, 2.0f, 0.0f,    1.0f, 2.0f, 1.0f,    1.0f, 2.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
	};


	matProj = Matrix_MakeProjection(90.0f, static_cast<float>(GetConsoleHeight()) / static_cast<float>(GetConsoleWidth()), 0.1f, 1000.0f);

	lightINF.x = 1.0f;
	lightINF.y = -100.0f;
	lightINF.z = 1.0f;
	
	scale = 1.0f;							
	_x = 0.65f, _y = 0.5f; 
	fThetaX = fThetaY = fThetaZ = 0.0f;
}

void NewGarphics::OnUserUpdate(float fElapsedTime)
{
	// Clear our console
	Fill(0, 0, GetConsoleWidth(), GetConsoleHeight());

	// Surface
	Fill(0, iConsoleHeight / 2, iConsoleWidth, iConsoleHeight, PIXEL_SOLID, FG_WHITE);

	// Move around axies
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

	// Scaling
	if (GetKey(L'Z').bHeld)		// Increace
		scale = (scale <= 1.5f) ? scale + 0.01f : scale;
	if (GetKey(L'X').bHeld)		// Decreace
		scale = (scale >= 0.5f) ? scale - 0.01f : scale;

	// Shifts
	if (GetKey(L'R').bHeld)		// Move to right
		_x += 0.01f;
	if (GetKey(L'F').bHeld)		// Move to left
		_x -= 0.01f;
	if (GetKey(L'T').bHeld)		// Move to up
		_y -= 0.01f;
	if (GetKey(L'G').bHeld)		// Move to down
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
	int16_t tri_color = FG_DARK_BLUE;
	int16_t count_tris = 0;
	for (auto& sh: shapes) 
	{
		// Take all triangles
		for (auto tri : sh.tris)
		{
			triangle triProjected, triTransformed;

			triTransformed.points[0] = MultiplyMatrixVector(WorldMatrix, tri.points[0]);
			triTransformed.points[1] = MultiplyMatrixVector(WorldMatrix, tri.points[1]);
			triTransformed.points[2] = MultiplyMatrixVector(WorldMatrix, tri.points[2]);		

			// Translate from 3D --> 2D
			triProjected.points[0] = MultiplyMatrixVector(matProj, triTransformed.points[0]);
			triProjected.points[1] = MultiplyMatrixVector(matProj, triTransformed.points[1]);
			triProjected.points[2] = MultiplyMatrixVector(matProj, triTransformed.points[2]);

			// Scaling to the size of the console
			for (int16_t i = 0; i < 3; i++)
			{
				triProjected.points[i].x += _x + t;
				triProjected.points[i].y += _y;

				triProjected.points[i].x *= 0.2f * static_cast<float>(GetConsoleWidth());
				triProjected.points[i].y *= 0.2f * static_cast<float>(GetConsoleHeight());

				// Counting barycenter
				barycenter += triProjected.points[i];
			}
			count_tris++;

			// Change color
			tri_color = (tri_color == FG_DARK_BLUE) ? FG_GREEN : FG_DARK_BLUE;
			triProjected.col = tri_color;

			vecTrianglesToRaster.push_back(triProjected);
		}

		// Get barycenter of figure
		barycenter /= count_tris * 3;

		// Sort triangles from back to front
		std::sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
			{
				float z1 = (t1.points[0].z + t1.points[1].z + t1.points[2].z) / 3.0f;
				float z2 = (t2.points[0].z + t2.points[1].z + t2.points[2].z) / 3.0f;
				return z1 < z2;
			});

		// Round all coord of points
		for (auto& tri : vecTrianglesToRaster)
		{
			for (int16_t i = 0; i < 3; i++)
			{
				tri.points[i].x = roundf(tri.points[i].x);
				tri.points[i].y = roundf(tri.points[i].y);
			}
		}

		// Draw
		DrawShadow(vecTrianglesToRaster, lightINF);
		//WarnockAlgorithm(vecTrianglesToRaster, 0.0f, iConsoleWidth - 1, 0.0f, iConsoleHeight - 1);

		std::vector<triangle> vecVisibleSurfaces;
		fPoint3D view_point = { static_cast<float>(iConsoleWidth) / 2.0f, static_cast<float>(iConsoleHeight) / 2.0f, -100.0f };

		vecVisibleSurfaces = RobertsAlgorithm(vecTrianglesToRaster, view_point, barycenter);
		RobertsAlgorithm(vecVisibleSurfaces, lightINF, barycenter, PIXEL_SOLID, FG_DARK_BLUE);

		//for (auto& tri : vecTrianglesToRaster)
		//{
		//	for (int16_t i = 0; i < 3; i++)
		//	{
		//		lines[i].x = tri.points[i].x;
		//		lines[i].y = tri.points[i].y;
		//	}

		//	// Выводим на экран

		//	//ShadingPolygonsScanLine(lines, PIXEL_SOLID, FG_DARK_BLUE);
		//	DrawPolygons(lines, PIXEL_SOLID, FG_GREY);
		//}
		
		t += 2.4f;
		barycenter = 0.0f;
		count_tris = 0;
		vecTrianglesToRaster.clear();
	}
}