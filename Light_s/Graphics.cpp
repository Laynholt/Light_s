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
	bool bKeyWasPressed = true;									// Else we cann't go to OnUserUpdate()

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

				bKeyWasPressed = true;
			}

			m_keyOldState[i] = m_keyNewState[i];
		}

		if (bKeyWasPressed)
		{
			OnUserUpdate(fElapsedTime);
			bKeyWasPressed = false;
		}

		// Update Title & Present Screen Buffer
		wchar_t s[256];
		swprintf_s(s, 256, L"%s - FPS: %3.2f", wsApp_name.c_str(), 1.0f / fElapsedTime);
		SetConsoleTitle(s);
		WriteConsoleOutput(hConsole, console, { iConsoleWidth, iConsoleHeight }, { 0,0 }, &rectWindow);
	}
}

int16_t Graphics::GetConsoleWidth()
{
	return iConsoleWidth;
}

int16_t Graphics::GetConsoleHeight()
{
	return iConsoleHeight;
}

Graphics::sKeyState& Graphics::GetKey(int16_t key_id)
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

void Graphics::DrawLineBresenham(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t c, int16_t col)
{
	int16_t x, y;
	int16_t deltaX, deltaY;
	int16_t signX, signY;
	int16_t balance;

	signX = (x2 > x1) ? 1 : -1;
	signY = (y2 > y1) ? 1 : -1;

	deltaX = (signX > 0) ? (x2 - x1) : (x1 - x2);
	deltaY = (signY > 0) ? (y2 - y1) : (y1 - y2);

	x = x1; y = y1;

	if (deltaX >= deltaY)
	{
		deltaY <<= 1;
		balance = deltaY - deltaX;
		deltaX <<= 1;

		while (x != x2)
		{
			Draw(x, y, ' ', BG_WHITE);
			if (balance >= 0)
			{
				y += signY;
				balance -= deltaX;
			}

			balance += deltaY;
			x += signX;
		}

		Draw(x, y, ' ', BG_WHITE);
	}

	else
	{
		deltaX <<= 1;
		balance = deltaX - deltaY;
		deltaY <<= 1;

		while (y != y2)
		{
			Draw(x, y, ' ', BG_WHITE);
			if (balance >= 0)
			{
				x += signX;
				balance -= deltaY;
			}

			balance += deltaX;
			y += signY;
		}

		Draw(x, y, ' ', BG_WHITE);
	}
}

void Graphics::DrawPolygons(std::vector<fPoint>& points, int16_t c, int16_t col)
{
	int16_t i;

	for (i = 0; i < points.size() - 1; i++)
	{
		DrawLineBresenham(round(points[i].x), round(points[i].y), round(points[i + 1].x), round(points[i + 1].y), c, col);
	}
	DrawLineBresenham(round(points[i].x), round(points[i].y), round(points[0].x), round(points[0].y), c, col);
}

void Graphics::Fill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t c, int16_t col)
{
	Clip(x1, y1);
	Clip(x2, y2);
	for (int16_t x = x1; x < x2; x++)
		for (int16_t y = y1; y < y2; y++)
			Draw(x, y, c, col);
}

void Graphics::Clip(int16_t& x, int16_t& y)
{
	if (x < 0) x = 0;
	else if (x >= iConsoleWidth) x = iConsoleWidth;

	if (y < 0) y = 0;
	else if (y >= iConsoleHeight) y = iConsoleHeight;
}
