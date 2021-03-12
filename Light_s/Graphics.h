//This library bases on the Javidx9's library: 
// https://github.com/OneLoneCoder/videos/blob/master/olcConsoleGameEngine.h

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
		// Struct for 2D graphics
	struct fPoint2D										// Point struct, which have X,Y coords
	{
		float x;
		float y;

	public:
		fPoint2D(): x(0.0f), y(0.0f) {}
		fPoint2D(float x, float y): x(x), y(y) {}

		fPoint2D& operator=(const fPoint2D& obj)
		{
			x = obj.x;
			y = obj.y;
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
		// Struct for 3D graphics
	struct fPoint3D										// Point struct, which have X,Y coords
	{
		float x, y, z, w;

	public:
		fPoint3D() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
		fPoint3D(float x, float y, float z, float w = 1.0f) : x(x), y(y), z(z), w(w) {}

		fPoint3D& operator=(const fPoint3D& obj)
		{
			x = obj.x;
			y = obj.y;
			z = obj.z;
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
			return fPoint3D(x + value, y / value, z / value);
		}

		float DotProduct(fPoint3D& v, float angle) {}
		float VectorLength() {}
	};

	// Drawing methods
public:
	void Draw(int16_t x, int16_t y, int16_t c = ' ', int16_t col = BG_WHITE);
	void DrawLineBresenham(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t c = ' ', int16_t col = BG_WHITE);
	void DrawPolygons(std::vector<fPoint2D>& points, int16_t c = ' ', int16_t col = BG_WHITE);

		// Clear our console
	void ClearConsole(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t c = PIXEL_SOLID, int16_t col = FG_BLACK);
	void Clip(int16_t& x, int16_t& y);

	void ShadingPolygonsScanLine(const std::vector<fPoint2D>& points, int16_t c = ' ', int16_t col = BG_WHITE);
	void ShadingPolygonsFloodFill(const std::vector<fPoint2D>& points, int16_t c = ' ', 
		int16_t col = BG_WHITE, int16_t col_edges = BG_WHITE);


	// Actions methods
public:
	void RotateLineAroundPoint(float x1, float y1, float& x2, float& y2, float& angle);		// x1, y1 - static coords
	void RotateLineAroundCenter(float& x1, float& y1, float& x2, float& y2, float& angle);
	void RotatePolygons(std::vector<fPoint2D>& points, float& angle);

	bool ScalingLine(fPoint2D& point1, fPoint2D& point2, float k);
	bool ScalingPolygons(std::vector<fPoint2D>& points, float k);
};

#endif // !_GRAPHICS_H_
