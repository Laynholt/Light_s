#include "Graphics.h"

Graphics::Graphics()
{
	iConsoleWidth = 120;
	iConsoleHeight = 60;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	console = nullptr;
	rectWindow = { 0 };

	std::memset(m_keyNewState, 0, 256 * sizeof(short));
	std::memset(m_keyOldState, 0, 256 * sizeof(short));
	std::memset(m_keys, 0, 256 * sizeof(sKeyState));

	wsApp_name = L"Light\'s";
}

Graphics::~Graphics()
{
	SetConsoleActiveScreenBuffer(hOriginalConsole);
	delete[] console;
}

void Graphics::ConstructConsole(int16_t width, int16_t height, int16_t font_w, int16_t font_h)
{
	if (hConsole == INVALID_HANDLE_VALUE)
	{
		Error(L"Handle error.");
		return;
	}

	iConsoleWidth = width;
	iConsoleHeight = height;

	rectWindow = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);

	// Set the size of the screen buffer
	COORD coord = { (short)iConsoleWidth, (short)iConsoleHeight };
	if (!SetConsoleScreenBufferSize(hConsole, coord))
		Error(L"SetConsoleScreenBufferSize");

	// Assign screen buffer to the console
	if (!SetConsoleActiveScreenBuffer(hConsole))
	{
		Error(L"SetConsoleActiveScreenBuffer");
		return;
	}

	// Set the font size now that the screen buffer has been assigned to the console
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = font_w;
	cfi.dwFontSize.Y = font_h;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	
	wcscpy_s(cfi.FaceName, L"Consolas");
	if (!SetCurrentConsoleFontEx(hConsole, false, &cfi))
		return Error(L"SetCurrentConsoleFontEx");

	// Get screen buffer info and check the maximum allowed window size. Return
	// error if exceeded, so user knows their dimensions/fontsize are too large
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
		return Error(L"GetConsoleScreenBufferInfo");
	if (iConsoleHeight > csbi.dwMaximumWindowSize.Y)
		return Error(L"Screen Height / Font Height Too Big");
	if (iConsoleWidth > csbi.dwMaximumWindowSize.X)
		return Error(L"Screen Width / Font Width Too Big");

	// Set Physical Console Window Size
	rectWindow = { 0, 0, (short)iConsoleWidth - 1, (short)iConsoleHeight - 1 };
	if (!SetConsoleWindowInfo(hConsole, TRUE, &rectWindow))
		return Error(L"SetConsoleWindowInfo");

	// Allocate memory for screen buffer
	console = new CHAR_INFO[iConsoleWidth * iConsoleHeight];
	memset(console, 0, sizeof(CHAR_INFO) * iConsoleWidth * iConsoleHeight);
}

void Graphics::Error(const wchar_t* msg)
{
	wchar_t buf[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
	SetConsoleActiveScreenBuffer(hOriginalConsole);
	wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
}

void Graphics::Loop()
{
	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	OnUserCreate();

	bool bExit = false;
	while (!bExit)
	{
		// Handle Timing
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsedTime = tp2 - tp1;

		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		// Handle Keyboard Input
		for (int16_t i = 0; i < 256; i++)
		{
			m_keyNewState[i] = GetAsyncKeyState(i);

			m_keys[i].bPressed = false;
			m_keys[i].bReleased = false;

			if (m_keyNewState[i] != m_keyOldState[i])
			{
				if (m_keyNewState[i] & 0x8000)
				{
					m_keys[i].bPressed = !m_keys[i].bHeld;
					m_keys[i].bHeld = true;
				}
				else
				{
					m_keys[i].bReleased = true;
					m_keys[i].bHeld = false;
				}
			}

			m_keyOldState[i] = m_keyNewState[i];
		}

		OnUserUpdate(fElapsedTime);

		// Update Title & Present Screen Buffer
		wchar_t s[256];
		swprintf_s(s, 256, L"%s - FPS: %3.2f", wsApp_name.c_str(), 1.0f / fElapsedTime);
		SetConsoleTitle(s);
		WriteConsoleOutput(hConsole, console, { iConsoleWidth, iConsoleHeight }, { 0,0 }, &rectWindow);
	}
}

int16_t Graphics::Get_Console_Width()
{
	return iConsoleWidth;
}

int16_t Graphics::Get_Console_Height()
{
	return iConsoleHeight;
}

Graphics::sKeyState& Graphics::Get_Key(int16_t key_id)
{
	return m_keys[key_id];
}

//########################################//
//-------------Graphic-2D-----------------//
//########################################//

void Graphics::Draw(int16_t x, int16_t y, int16_t c, int16_t col)
{
	if (x >= 0 && x < iConsoleWidth && y >= 0 && y < iConsoleHeight)
	{
		console[y * iConsoleWidth + x].Char.UnicodeChar = c;
		console[y * iConsoleWidth + x].Attributes = col;
	}
}

void Graphics::DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t c, int16_t col)
{

}

void Graphics::DrawPolygons(std::vector<fPoint>& points, int16_t c, int16_t col)
{
}
