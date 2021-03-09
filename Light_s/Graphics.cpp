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

int16_t Graphics::ConstructConsole(int16_t width, int16_t height, int16_t font_w, int16_t font_h)
{
	if (hConsole == INVALID_HANDLE_VALUE)
		return Error(L"Handle error.");

	iConsoleWidth = width;
	iConsoleHeight = height;

	rectWindow = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);

	// Set the size of the screen buffer
	COORD coord = { (int16_t)iConsoleWidth, (int16_t)iConsoleHeight };
	if (!SetConsoleScreenBufferSize(hConsole, coord))
		Error(L"SetConsoleScreenBufferSize");

	// Assign screen buffer to the console
	if (!SetConsoleActiveScreenBuffer(hConsole))
		return Error(L"SetConsoleActiveScreenBuffer");

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
	rectWindow = { 0, 0, (int16_t)iConsoleWidth - 1, (int16_t)iConsoleHeight - 1 };
	if (!SetConsoleWindowInfo(hConsole, TRUE, &rectWindow))
		return Error(L"SetConsoleWindowInfo");

	// Allocate memory for screen buffer
	console = new CHAR_INFO[iConsoleWidth * iConsoleHeight];
	memset(console, 0, sizeof(CHAR_INFO) * iConsoleWidth * iConsoleHeight);

	return 0;
}

int16_t Graphics::Error(const wchar_t* msg)
{
	wchar_t buf[256];

	SetConsoleDefault();

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
	SetConsoleActiveScreenBuffer(hOriginalConsole);
	wprintf(L"ERROR: %s\n\t%s\n", msg, buf);

	return 1;
}

void Graphics::SetConsoleDefault()
{
	// Font 14; w 85; h 25

	// Set Font
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 8;
	cfi.dwFontSize.Y = 14;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, L"Lucida Console");
	SetCurrentConsoleFontEx(hConsole, false, &cfi);

	// Set the size of the screen buffer
	COORD coord = { 106, 26 };
	SetConsoleScreenBufferSize(hConsole, coord);

	// Assign screen buffer to the console
	SetConsoleActiveScreenBuffer(hConsole);

	// Set Physical Console Window Size
	rectWindow = { 0, 0, 105, 25 };
	SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);
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

void Graphics::ShadingPolygons(const std::vector<fPoint>& points, int16_t c, int16_t col)
{
	std::vector<iEdgeScanLine> edges(points.size());
	int16_t min_y, max_y;
	int16_t min_x, max_x;

	std::vector<int16_t> scanex;

	min_y = max_y = round(points[0].y);

	for (int16_t i = 0; i < points.size(); i++)
	{
		edges[i].x1 = round(points[i].x);
		edges[i].y1 = round(points[i].y);
		edges[i].x2 = ((i + 1) == points.size()) ? round(points[0].x) : round(points[i + 1].x);
		edges[i].y2 = ((i + 1) == points.size()) ? round(points[0].y) : round(points[i + 1].y);
		edges[i].del_x = edges[i].x2 - edges[i].x1;
		edges[i].del_y = edges[i].y2 - edges[i].y1;
		edges[i].del_xy = (edges[i].del_y == 0) ? 0 : edges[i].del_x / edges[i].del_y;
		edges[i].del_yx = (edges[i].del_x == 0) ? 0 : edges[i].del_y / edges[i].del_x;

		if (edges[i].y2 > max_y)
			max_y = edges[i].y2;
		else if (edges[i].y2 < min_y)
			min_y = edges[i].y2;
	}


	for (int16_t y = min_y; y < max_y; y++)
	{
		for (int16_t i = 0; i < points.size(); i++)
		{
			if ((edges[i].y1 >= y && edges[i].y2 < y) || (edges[i].y1 < y && edges[i].y2 >= y))
			{
				// Count X
				scanex.push_back(edges[i].x1 + edges[i].del_xy * (y - edges[i].y1));
			}
		}

		if (scanex.size())
		{
			std::sort(scanex.begin(), scanex.end());

			for (int16_t i = 0; i < scanex.size() - 1; i += 2)
			{
				DrawLineBresenham(scanex[i], y, scanex[i + 1], y, c, col);
			}

			scanex.clear();
		}
	}
}

void Graphics::RotateLineAroundPoint(float x1, float y1, float& x2, float& y2, float& angle)
{
	float eps = PI / 180;													// Convert to radian

	x2 = x1 + (x2 - x1) * cos(angle * eps) - (y2 - y1) * sin(angle * eps);  // Rotate angle
	y2 = y1 + (x2 - x1) * sin(angle * eps) + (y2 - y1) * cos(angle * eps);  // Ratate angle
}

void Graphics::RotateLineAroundCenter(float& x1, float& y1, float& x2, float& y2, float& angle)
{
	float eps = PI / 180;						// Convert to radian

	float x0 = (x1 + x2) / 2;					// Get X center
	float y0 = (y1 + y2) / 2;					// Get Y center

	// First point
	x1 = -sin(angle * eps) * (y1 - y0) + cos(angle * eps) * (x1 - x0) + x0;
	y1 = cos(angle * eps) * (y1 - y0) + sin(angle * eps) * (x1 - x0) + y0;

	// Second point
	x2 = -sin(angle * eps) * (y2 - y0) + cos(angle * eps) * (x2 - x0) + x0;
	y2 = cos(angle * eps) * (y2 - y0) + sin(angle * eps) * (x2 - x0) + y0;
}

void Graphics::RotatePolygons(std::vector<fPoint>& points, float& angle)
{
	float c_x, c_y;
	c_x = c_y = 0.0f;

	for (int16_t i = 0; i < points.size(); i++)
	{
		c_x += points[i].x;
		c_y += points[i].y;
	}

	c_x /= points.size();
	c_y /= points.size();

	for (int16_t i = 0; i < points.size(); i++)
	{
		RotateLineAroundPoint(c_x, c_y, points[i].x, points[i].y, angle);
	}
}

bool Graphics::ScalingLine(fPoint& point1, fPoint& point2, float k)
{
	fPoint centers;

	// Center of the line
	centers.x = (point1.x + point2.x) / 2;
	centers.y = (point1.y + point2.y) / 2;

	if ((point1.x >= 0 && point1.x <= GetConsoleWidth() && point1.y >= 0 && point1.y <= GetConsoleHeight())
		|| (point2.x >= 0 && point2.x <= GetConsoleWidth() && point2.y >= 0 && point2.y <= GetConsoleHeight()) || k < 1.0f)
	{
		if ((fabs(point1.x - point2.x) > 0.01f
			&& fabs(point1.y - point2.y) > 0.01f) || k > 1.0f)
		{
			point1.x = ((point1.x - centers.x) * k) + centers.x;
			point2.x = ((point2.x - centers.x) * k) + centers.x;
			point1.y = ((point1.y - centers.y) * k) + centers.y;
			point2.y = ((point2.y - centers.y) * k) + centers.y;

			return false;
		}
	}

	return true;
}

bool Graphics::ScalingPolygons(std::vector<fPoint>& points, float k)
{
	float c_x, c_y;									// Center coords
	fPoint near_point;

	c_x = c_y = 0.0f;
	near_point = points[0];

	for (int16_t i = 0; i < points.size(); i++)
	{
		c_x += points[i].x;
		c_y += points[i].y;
	}

	c_x /= points.size();
	c_y /= points.size();

	int16_t count = 0;								// Number of visible points

	for (int16_t i = 0; i < points.size(); i++)
	{
		// Counting number of visible points
		if (points[i].x >= 0 && points[i].x <= GetConsoleWidth() && points[i].y >= 0 && points[i].y <= GetConsoleHeight())
			count++;

		// Find near point about center
		if (fabs(points[i].x - c_x) <= near_point.x && fabs(points[i].y - c_y) <= near_point.y)
			near_point = points[i];
	}

	if (((fabs(near_point.x - c_x) > 0.01f)
		&& (fabs(near_point.y - c_y) > 0.01f)) || k > 1.0f)
	{
		if (count >= 1 || k < 1.0f)
		{
			for (int16_t i = 0; i < points.size(); i++)
			{
				points[i].x = ((points[i].x - c_x) * k) + c_x;
				points[i].y = ((points[i].y - c_y) * k) + c_y;
			}

			return false;
		}
	}

	return true;
}
