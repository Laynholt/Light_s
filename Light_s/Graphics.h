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

#include <vector>

#include <cmath>
#include <algorithm>

const float PI = 3.14159f;

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

	void ConstructConsole(int16_t width, int16_t height, int16_t font_w, int16_t font_h);

	int16_t GetConsoleWidth();
	int16_t GetConsoleHeight();
	sKeyState& GetKey(int16_t key_id);

protected:
	void Error(const wchar_t* msg);

	virtual void OnUserCreate() = 0;
	virtual void OnUserUpdate(float fElapsedTime) = 0;

public:
	void Loop();

//---Draw---//
	// Drawing variables & structures
protected:
	struct fPoint										// Point struct, which have X,Y coords
	{
		float x;
		float y;

	public:
		fPoint(): x(0.0f), y(0.0f) {}
		fPoint& operator=(const fPoint& obj)
		{
			x = obj.x;
			y = obj.y;
			return *this;
		}
	};

	// Drawing methods
public:
	void Draw(int16_t x, int16_t y, int16_t c, int16_t col);
	void DrawLineBresenham(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t c, int16_t col);
	void DrawPolygons(std::vector<fPoint>& points, int16_t c, int16_t col);

		// Clear our console
	void Fill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t c, int16_t col);
	void Clip(int16_t& x, int16_t& y);
};

#endif // !_GRAPHICS_H_
