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
	void Error(const wchar_t* msg);

	int16_t Get_Console_Width();
	int16_t Get_Console_Height();
	sKeyState& Get_Key(int16_t key_id);

	virtual void OnUserCreate() = 0;
	virtual void OnUserUpdate(float fElapsedTime) = 0;

private:
	void Loop();
};

#endif // !_GRAPHICS_H_
