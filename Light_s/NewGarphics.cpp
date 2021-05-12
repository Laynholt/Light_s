#include "NewGarphics.h"

void NewGarphics::OnUserCreate()
{
	shapes.resize(2);

	shapes[0].tris =
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

		//// Основание
		//	{ 0.0f, 0.0f, 0.0f,    2.0f, 0.0f, 0.0f,    1.0f, 0.0f, 2.0f },

		//	// Перед                                                     
		//	{ 0.0f, 0.0f, 0.0f,    1.0f, 2.0f, 1.0f,    2.0f, 0.0f, 0.0f },

		//	// Право                                                       
		//	{ 2.0f, 0.0f, 0.0f,    1.0f, 2.0f, 1.0f,    1.0f, 0.0f, 2.0f },

		//	// Лево                                                    
		//	{ 1.0f, 0.0f, 2.0f,    1.0f, 2.0f, 1.0f,    0.0f, 0.0f, 0.0f },

			
	};

	shapes[1].tris =
	{
		//// Основание
		//	{ 0.0f, 0.0f, 0.0f,    2.0f, 0.0f, 0.0f,    1.0f, 0.0f, 2.0f },

		//	// Перед                                                     
		//	{ 0.0f, 0.0f, 0.0f,    1.0f, 2.0f, 1.0f,    2.0f, 0.0f, 0.0f },

		//	// Право                                                       
		//	{ 2.0f, 0.0f, 0.0f,    1.0f, 2.0f, 1.0f,    1.0f, 0.0f, 2.0f },

		//	// Лево                                                    
		//	{ 1.0f, 0.0f, 2.0f,    1.0f, 2.0f, 1.0f,    0.0f, 0.0f, 0.0f },

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
	};


	matProj = Matrix_MakeProjection(90.0f, static_cast<float>(GetConsoleHeight()) / static_cast<float>(GetConsoleWidth()), 0.1f, 1000.0f);

	//light.x = 1.0f;
	//light.y = -100.0f;
	//light.z = 1.0f;

	light.x = iConsoleWidth / 2;
	light.y = -100.0f;
	light.z = 5.0f;
	
	scale = 1.0f;							
	//_x = 2.35f, _y = 1.2f; 
	_x = 0.5f; _y = 0.5f; _z = 5.0f;
	fThetaX = fThetaY = fThetaZ = 0.0f;
}

void NewGarphics::OnUserUpdate(float fElapsedTime)
{
	// Clear our console
	Fill(0, 0, GetConsoleWidth(), GetConsoleHeight());

	// Surface
	Fill(0, iConsoleHeight / 2, iConsoleWidth, iConsoleHeight, PIXEL_SOLID, FG_DARK_YELLOW);

	//// Move around axies
	if (GetKey(L'W').bHeld)
		fThetaX += 8.0f * fElapsedTime;
	if (GetKey(L'S').bHeld)
		fThetaX -= 8.0f * fElapsedTime;
	if (GetKey(L'A').bHeld)
		fThetaY += 8.0f * fElapsedTime;
	if (GetKey(L'D').bHeld)
		fThetaY -= 8.0f * fElapsedTime;
	if (GetKey(L'Q').bHeld)
		fThetaZ += 8.0f * fElapsedTime;
	if (GetKey(L'E').bHeld)
		fThetaZ -= 8.0f * fElapsedTime;

	// Scaling
	if (GetKey(L'Z').bHeld)		// Increace
		scale = (scale <= 1.5f) ? scale + 0.01f : scale;
	if (GetKey(L'X').bHeld)		// Decreace
		scale = (scale >= 0.5f) ? scale - 0.01f : scale;

	// Shifts
	//if (GetKey(L'R').bHeld)		// Move to right
	//	_x += 0.01f;
	//if (GetKey(L'F').bHeld)		// Move to left
	//	_x -= 0.01f;
	//if (GetKey(L'T').bHeld)		// Move to up
	//	_y -= 0.01f;
	//if (GetKey(L'G').bHeld)		// Move to down
	//	_y += 0.01f;
	//if (GetKey(L'Y').bHeld)		// Move to up
	//	_z = (_z > 1.0f) ? _z - 0.1f : _z;
	//if (GetKey(L'H').bHeld)		// Move to down
	//	_z += 0.1f;

	if (GetKey(VK_LBUTTON).bHeld)
	{
		if (GetMouseX() >= 0.9f * iConsoleWidth)		// Move to right
			_x += 0.005f;
		if (GetMouseX() <= 0.2f * iConsoleWidth)		// Move to left
			_x -= 0.005f;
		if (GetMouseY() <= 0.2f * iConsoleHeight)		// Move to up
			_y -= 0.005f;
		if (GetMouseY() >= 0.9f * iConsoleHeight)		// Move to down
			_y += 0.005f;
		if ((GetMouseX() >= 0.40f * iConsoleWidth) && (GetMouseX() <= 0.60f * iConsoleWidth) 
			&& (GetMouseY() >= 0.40f * iConsoleHeight) && (GetMouseY() <= 0.60f * iConsoleHeight))		// Move to forward
			_z += 0.1f;	
	}
	else if (GetKey(VK_RBUTTON).bHeld)
	{
		if ((GetMouseX() >= 0.40f * iConsoleWidth) && (GetMouseX() <= 0.60f * iConsoleWidth)
			&& (GetMouseY() >= 0.40f * iConsoleHeight) && (GetMouseY() <= 0.60f * iConsoleHeight))		// Move to back
			_z = (_z > 1.0f) ? _z - 0.1f : _z;
	}


	mat4x4 matRotX, matRotY, matRotZ;
	matRotX = Matrix_MakeRotationX(fThetaX * 0.5f);
	matRotY = Matrix_MakeRotationY(fThetaY * 0.5f);
	matRotZ = Matrix_MakeRotationZ(fThetaZ * 0.5f);

	mat4x4 ScalingMatrix;
	ScalingMatrix = Matrix_MakeScale(scale, scale, scale);

	mat4x4 TranslationMatrix;
	TranslationMatrix = Matrix_MakeTranslation(0.0f, 0.0f, _z);

	mat4x4 WorldMatrix;
	WorldMatrix = Matrix_MakeIdentity();
	WorldMatrix = matRotY * matRotX * matRotZ * ScalingMatrix * TranslationMatrix;

	std::vector<triangle> vecTrianglesToRaster;

	float  t = 0.0f;					// move X coor for another figure
	int16_t tri_color = FG_DARK_GREEN;
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

			// Scale into view, we moved the normalising into cartesian space
			// out of the matrix.vector function from the previous videos, so
			// do this manually
			triProjected.points[0] = triProjected.points[0] / triProjected.points[0].w;
			triProjected.points[1] = triProjected.points[1] / triProjected.points[1].w;
			triProjected.points[2] = triProjected.points[2] / triProjected.points[2].w;

			// X/Y are inverted so put them back
			triProjected.points[0].x *= -1.0f;
			triProjected.points[1].x *= -1.0f;
			triProjected.points[2].x *= -1.0f;
			triProjected.points[0].y *= -1.0f;
			triProjected.points[1].y *= -1.0f;
			triProjected.points[2].y *= -1.0f;

			// Scaling to the size of the console
			for (int16_t i = 0; i < 3; i++)
			{
				triProjected.points[i].x += _x + t;
				triProjected.points[i].y += _y;

				triProjected.points[i].x *= 0.5f * static_cast<float>(GetConsoleWidth());
				triProjected.points[i].y *= 0.5f * static_cast<float>(GetConsoleHeight());

				// Counting barycenter
				barycenter += triProjected.points[i];
			}
			count_tris++;

			// Change color
			tri_color++;
			if (tri_color == FG_GREY) tri_color++;
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
				return z1 > z2; // was >
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
		DrawShadow(vecTrianglesToRaster, light);
		//WarnockAlgorithm(vecTrianglesToRaster, 0.0f, iConsoleWidth - 1, 0.0f, iConsoleHeight - 1);

		std::vector<triangle> vecVisibleSurfaces;
		fPoint3D view_point = { static_cast<float>(iConsoleWidth) / 2.0f, static_cast<float>(iConsoleHeight) / 2.0f, -100.0f };

		vecVisibleSurfaces = RobertsAlgorithm(vecTrianglesToRaster, view_point, barycenter);
		RobertsAlgorithm(vecVisibleSurfaces, light, barycenter, PIXEL_SOLID, FG_DARK_BLUE);

		//PainterAlgorithm(vecTrianglesToRaster);
		
		t += 1.0f;
		barycenter = 0.0f;
		count_tris = 0;
		vecTrianglesToRaster.clear();
	}
}