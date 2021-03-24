//This library bases on the Javidx9's library: 
// https://github.com/OneLoneCoder/videos/blob/master/olcConsoleGameEngine.h
// https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_olcEngine3D_Part3.cpp

#ifndef UNICODE
#error Please enable UNICODE for your compiler! VS: Project Properties -> General -> \
Character Set -> Use Unicode.
#endif

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include <Windows.h>
#include <cstdint>
#include <iostream>

#include <chrono>
//#include <thread>

#include <queue>
#include <vector>

#include <cmath>
#include <algorithm>

constexpr float PI = 3.14159f;

	// Enum of colors for drawing
enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007,
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

//###################//
	// Main Class
//###################//

class Graphics
{
	// Main variables for project
protected:
	int16_t iConsoleWidth, iConsoleHeight;				// Size of console
	HANDLE hConsole;									// Current output handle
	HANDLE hOriginalConsole;							// Original handle (need when we got some error)
	CHAR_INFO* console;									// Array of characters
	SMALL_RECT rectWindow;								
	std::wstring wsApp_name;

		// Keyboard using
	struct sKeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	};

	int16_t m_keyOldState[256];
	int16_t m_keyNewState[256];
	sKeyState m_keys[256];

	// Main methods
public:
	Graphics();
	~Graphics();

	int16_t ConstructConsole(int16_t width, int16_t height, int16_t font_w, int16_t font_h);

	int16_t GetConsoleWidth();
	int16_t GetConsoleHeight();
	sKeyState& GetKey(int16_t key_id);

protected:
	int16_t Error(const wchar_t* msg);

	virtual void OnUserCreate() = 0;
	virtual void OnUserUpdate(float fElapsedTime) = 0;

private:
	void SetConsoleDefault();

public:
	void Loop();

//---Draw---//
	// Drawing variables & structures
protected:
		// Structs for 2D graphics
	struct fPoint3D;

	struct mat3x3
	{
		float m[3][3] = { 0 };
	};
	struct fPoint2D										// Point struct, which have X,Y coords
	{
		float x;
		float y;
		float w;

	public:
		fPoint2D(): x(0.0f), y(0.0f), w(0.0f) {}
		fPoint2D(float x, float y, float w = 1.0f): x(x), y(y), w(w) {}

		fPoint2D& operator=(const fPoint2D& obj)
		{
			x = obj.x;
			y = obj.y;
			w = obj.w;
			return *this;
		}
		fPoint2D& operator=(const fPoint3D& obj)
		{
			x = obj.x;
			y = obj.y;
			w = obj.w;
			return *this;
		}
		fPoint2D& operator+=(const fPoint2D& obj)
		{
			x += obj.x;
			y += obj.y;
			return *this;
		}
		fPoint2D& operator-=(const fPoint2D& obj)
		{
			x -= obj.x;
			y -= obj.y;
			return *this;
		}
		fPoint2D& operator*=(float value)
		{
			x *= value;
			y *= value;
			return *this;
		}
		fPoint2D& operator/=(float value)
		{
			x /= value;
			y /= value;
			return *this;
		}

		fPoint2D operator+(const fPoint2D& obj)
		{
			return fPoint2D(x + obj.x, y + obj.y);
		}
		fPoint2D operator-(const fPoint2D& obj)
		{
			return fPoint2D(x - obj.x, y - obj.y);
		}
		fPoint2D operator*(float value)
		{
			return fPoint2D(x * value, y * value);
		}
		fPoint2D operator/(float value)
		{
			return fPoint2D(x / value, y / value);
		}

		void MultiplyMatrixVector(mat3x3& m)
		{
			x = x * m.m[0][0] + x * m.m[1][0] + x * m.m[2][0];
			y = y * m.m[0][1] + y * m.m[1][1] + y * m.m[2][1];
		}
	};
		// Struct for ScanLine algorithm
	struct iEdgeScanLine
	{
		int16_t x1, x2, y1, y2;
		float del_x, del_y, del_xy;

		iEdgeScanLine()
		{
			x1 = x2 = y1 = y2 = 0;
			del_x = del_y = del_xy = 0.0f;
		}

		iEdgeScanLine& operator=(const iEdgeScanLine& obj)
		{
			x1 = obj.x1;
			x2 = obj.x2;
			y1 = obj.y1;
			y2 = obj.y2;
			del_x = obj.del_x;
			del_y = obj.del_y;
			del_xy = obj.del_xy;
			return *this;
		}
	};
		// Structs for 3D graphics
	struct mat4x4
	{
		float m[4][4] = { 0 };

		mat4x4 operator*(mat4x4& _m)
		{
			mat4x4 matrix;
			for (int16_t c = 0; c < 4; c++)
				for (int16_t r = 0; r < 4; r++)
					matrix.m[r][c] = m[r][0] * _m.m[0][c] + m[r][1] * _m.m[1][c] + m[r][2] * _m.m[2][c] + m[r][3] * _m.m[3][c];
			return matrix;
		}
	};
	struct fPoint3D										// Point struct, which have X,Y coords
	{
		float x, y, z, w;

	public:
		fPoint3D() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
		fPoint3D(float x, float y, float z) : x(x), y(y), z(z), w(1.0f) {}

		fPoint3D& operator=(const fPoint3D& obj)
		{
			x = obj.x;
			y = obj.y;
			z = obj.z;
			w = obj.w;
			return *this;
		}
		fPoint3D& operator=(const fPoint2D& obj)
		{
			x = obj.x;
			y = obj.y;
			z = 0.0f;
			w = obj.w;
			return *this;
		}
		fPoint3D& operator+=(const fPoint3D& obj)
		{
			x += obj.x;
			y += obj.y;
			z += obj.z;
			return *this;
		}
		fPoint3D& operator-=(const fPoint3D& obj)
		{
			x -= obj.x;
			y -= obj.y;
			z -= obj.z;
			return *this;
		}
		fPoint3D& operator*=(float value)
		{
			x *= value;
			y *= value;
			z *= value;
			return *this;
		}
		fPoint3D& operator/=(float value)
		{
			x /= value;
			y /= value;
			z /= value;
			return *this;
		}

		fPoint3D operator+(const fPoint3D& obj)
		{
			return fPoint3D(x + obj.x, y + obj.y, z + obj.z);
		}
		fPoint3D operator-(const fPoint3D& obj)
		{
			return fPoint3D(x - obj.x, y - obj.y, z - obj.z);
		}
		fPoint3D operator*(float value)
		{
			return fPoint3D(x * value, y * value, z * value);
		}
		fPoint3D operator/(float value)
		{
			return fPoint3D(x / value, y / value, z / value);
		}
	};
	struct triangle
	{
		fPoint3D p[3];

		triangle() {};
		triangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
		{
			p[0].x = x1; p[0].y = y1; p[0].z = z1;
			p[1].x = x2; p[1].y = y2; p[1].z = z2;
			p[2].x = x3; p[2].y = y3; p[2].z = z3;
		}
	};

	struct mesh
	{
		std::vector<triangle> tris;
	};


	// Drawing methods
public:
	void Draw(int16_t x, int16_t y, int16_t c = ' ', int16_t col = BG_WHITE);
	void DrawLineBresenham(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t c = ' ', int16_t col = BG_WHITE);
	void DrawPolygons(std::vector<fPoint2D>& points, int16_t c = ' ', int16_t col = BG_WHITE);

		// Clear our console
	void ClearConsole(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t c = PIXEL_SOLID, int16_t col = FG_BLACK);
	void Clip(int16_t& x, int16_t& y);

	void ShadingPolygonsScanLine(const std::vector<fPoint2D>& points, int16_t c = ' ', int16_t col = BG_WHITE,
		int16_t y_min = -1, int16_t y_max = -1, int16_t x_min = -1, int16_t x_max = -1);
	void ShadingPolygonsFloodFill(const std::vector<fPoint2D>& points, int16_t c = ' ', 
		int16_t col = BG_WHITE, int16_t col_edges = BG_WHITE);


	// Actions methods
public:
	void RotateLineAroundPoint(float x1, float y1, float& x2, float& y2, float& angle);		// x1, y1 - static coords
	void RotateLineAroundCenter(float& x1, float& y1, float& x2, float& y2, float& angle);
	void RotatePolygons(std::vector<fPoint2D>& points, float& angle);

	bool ScalingLine(fPoint2D& point1, fPoint2D& point2, float k);
	bool ScalingPolygons(std::vector<fPoint2D>& points, float k);

	void WarnockAlgorithm(std::vector<triangle>& vecTrianglesToRaster, float _left_x,
		float _right_x, float _top_y, float _bottom_y);

	void MoveTo2D(std::vector<fPoint2D>& points, mat3x3& m);

	// Matrix methods (Use this for 3D)
public:
	float Vector_DotProduct(fPoint3D& v1, fPoint3D& v2);
	float Vector_Length(fPoint3D& v);
	fPoint3D Vector_Normalise(fPoint3D& v);
	fPoint3D Vector_CrossProduct(fPoint3D& v1, fPoint3D& v2);
	
	fPoint2D MultiplyMatrixVector(mat3x3& m, fPoint2D& v);
	fPoint3D MultiplyMatrixVector(mat4x4& m, fPoint3D& v);

	mat4x4 Matrix_MakeIdentity();
	mat4x4 Matrix_MakeRotationX(float fAngleRad);
	mat4x4 Matrix_MakeRotationY(float fAngleRad);
	mat4x4 Matrix_MakeRotationZ(float fAngleRad);
	mat4x4 Matrix_MakeScale(float x, float y, float z);
	mat4x4 Matrix_MakeTranslation(float x, float y, float z);
	mat4x4 Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar);
	mat4x4 Matrix_MultiplyMatrix(mat4x4& m1, mat4x4& m2);
};

#endif // !_GRAPHICS_H_
