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

	int16_t Get_Console_Width();
	int16_t Get_Console_Height();
	sKeyState& Get_Key(int16_t key_id);

protected:
	void Error(const wchar_t* msg);

	virtual void OnUserCreate() = 0;
	virtual void OnUserUpdate(float fElapsedTime) = 0;

private:
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
	void DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t c, int16_t col);
	void DrawPolygons(std::vector<fPoint>& points, int16_t c, int16_t col);

};

#endif // !_GRAPHICS_H_
