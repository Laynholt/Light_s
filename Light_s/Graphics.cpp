#include "Graphics.h"

Graphics::Graphics()
{
	iConsoleWidth = 120;
	iConsoleHeight = 60;

	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

	console = nullptr;
	rectWindow = { 0 };

	std::memset(m_keyNewState, 0, 256 * sizeof(short));
	std::memset(m_keyOldState, 0, 256 * sizeof(short));
	std::memset(m_keys, 0, 256 * sizeof(sKeyState));

	std::memset(m_mouse, 0, 5 * sizeof(sKeyState));
	std::memset(m_mouseOldState, 0, 5 * sizeof(bool));
	std::memset(m_mouseNewState, 0, 5 * sizeof(bool));
	m_mousePosX = 0;
	m_mousePosY = 0;

	m_bConsoleInFocus = true;

	wsApp_name = L"Light\'s";
}

Graphics::~Graphics()
{
	SetConsoleActiveScreenBuffer(hOriginalConsole);
	delete[] console;
}

int16_t Graphics::ConstructConsole(int16_t width, int16_t height, int16_t font_w, int16_t font_h, std::wstring Console_name)
{
	wsApp_name = Console_name;

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

	// Set flags to allow mouse input		
	if (!SetConsoleMode(m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		return Error(L"SetConsoleMode");

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


		// Handle Mouse Input - Check for window events
		INPUT_RECORD inBuf[32];
		DWORD events = 0;
		GetNumberOfConsoleInputEvents(m_hConsoleIn, &events);
		if (events > 0)
			ReadConsoleInput(m_hConsoleIn, inBuf, events, &events);

		// Handle events - we only care about mouse clicks and movement
		// for now
		for (DWORD i = 0; i < events; i++)
		{
			switch (inBuf[i].EventType)
			{
			case FOCUS_EVENT:
			{
				m_bConsoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
			}
			break;

			case MOUSE_EVENT:
			{
				switch (inBuf[i].Event.MouseEvent.dwEventFlags)
				{
				case MOUSE_MOVED:
				{
					m_mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
					m_mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
				}
				break;

				case 0:
				{
					for (int16_t m = 0; m < 5; m++)
						m_mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;

				}
				break;

				default:
					break;
				}
			}
			break;

			default:
				break;
				// We don't care just at the moment
			}
		}

		for (int16_t m = 0; m < 5; m++)
		{
			m_mouse[m].bPressed = false;
			m_mouse[m].bReleased = false;

			if (m_mouseNewState[m] != m_mouseOldState[m])
			{
				if (m_mouseNewState[m])
				{
					m_mouse[m].bPressed = true;
					m_mouse[m].bHeld = true;
				}
				else
				{
					m_mouse[m].bReleased = true;
					m_mouse[m].bHeld = false;
				}
			}

			m_mouseOldState[m] = m_mouseNewState[m];
		}


		if (bKeyWasPressed)
		{
			OnUserUpdate(fElapsedTime);
			//bKeyWasPressed = false;
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

void Graphics::Draw(int16_t x, int16_t y, int16_t sym, int16_t col)
{
	if (x >= 0 && x < iConsoleWidth && y >= 0 && y < iConsoleHeight)
	{
		console[y * iConsoleWidth + x].Char.UnicodeChar = sym;
		console[y * iConsoleWidth + x].Attributes = col;
	}
}

void Graphics::DrawLineBresenham(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t sym, int16_t col)
{
	/*	dx = (x2-x1)
		dy = (y2-y1)

		f(x,y) = dy*X - dx*Y - (x1*dy - y1*dx)

		f(M) = dy - dx/2
		f(T) = f(M) + dy - dx
		f(B) = f(M) + dy
	*/

	int16_t x, y;
	int16_t deltaX, deltaY;
	int16_t signX, signY;
	int16_t balance;

	signX = (x2 > x1) ? 1 : -1;
	signY = (y2 > y1) ? 1 : -1;

	deltaX = (signX > 0) ? (x2 - x1) : (x1 - x2);
	deltaY = (signY > 0) ? (y2 - y1) : (y1 - y2);

	x = x1; y = y1;

	if (deltaX >= deltaY)				// We should use larger axis [X>=Y]
	{
		deltaY <<= 1;
		balance = deltaY - deltaX;		// f(M)
		deltaX <<= 1;

		while (x != x2)
		{
			Draw(x, y, sym, col);
			if (balance >= 0)
			{
				y += signY;
				balance -= deltaX;		// f(T) = f(M) - dx => half part f(T) [still need dy]
			}

			balance += deltaY;			// f(M) + dy => f(B) or part of f(T)
			x += signX;
		}

		Draw(x, y, sym, col);
	}

	else								// Similarly for axis [Y>X]
	{
		deltaX <<= 1;
		balance = deltaX - deltaY;
		deltaY <<= 1;

		while (y != y2)
		{
			Draw(x, y, sym, col);
			if (balance >= 0)
			{
				x += signX;
				balance -= deltaY;
			}

			balance += deltaX;
			y += signY;
		}

		Draw(x, y, sym, col);
	}
}

void Graphics::DrawPolygons(std::vector<fPoint2D>& points, int16_t sym, int16_t col)
{
	size_t i;

	for (i = 0; i < points.size() - 1; i++)
	{
		DrawLineBresenham(roundf(points[i].x), roundf(points[i].y), roundf(points[i + 1].x), roundf(points[i + 1].y), sym, col);
	}
	DrawLineBresenham(roundf(points[i].x), roundf(points[i].y), roundf(points[0].x), roundf(points[0].y), sym, col);
}

void Graphics::Fill(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t sym, int16_t col)
{
	Clip(x1, y1);
	Clip(x2, y2);
	for (int16_t x = x1; x <= x2; x++)
		for (int16_t y = y1; y <= y2; y++)
			Draw(x, y, sym, col);
}

void Graphics::Clip(int16_t& x, int16_t& y)
{
	if (x < 0) x = 0;
	else if (x >= iConsoleWidth) x = iConsoleWidth;

	if (y < 0) y = 0;
	else if (y >= iConsoleHeight) y = iConsoleHeight;
}

void Graphics::ShadingPolygonsScanLine(const std::vector<fPoint2D>& points, int16_t sym, int16_t col, int16_t y_min, int16_t y_max,
	int16_t x_min, int16_t x_max)
{
	std::vector<iEdgeScanLine> edges(points.size());
	int16_t min_y, max_y;

	std::vector<int16_t> scanex;

	min_y = max_y = round(points[0].y);

	for (size_t i = 0; i < points.size(); i++)
	{
		edges[i].x1 = round(points[i].x);
		edges[i].y1 = round(points[i].y);
		edges[i].x2 = ((i + 1) == points.size()) ? round(points[0].x) : round(points[i + 1].x);
		edges[i].y2 = ((i + 1) == points.size()) ? round(points[0].y) : round(points[i + 1].y);
		edges[i].del_x = edges[i].x2 - edges[i].x1;
		edges[i].del_y = edges[i].y2 - edges[i].y1;
		edges[i].del_xy = (edges[i].del_y == 0) ? 0 : edges[i].del_x / edges[i].del_y;

		if (edges[i].y2 > max_y)
			max_y = edges[i].y2;
		else if (edges[i].y2 < min_y)
			min_y = edges[i].y2;
	}

	// For Warnock Algorithm
	y_min = (y_min == -1) ? 0 : y_min;
	y_max = (y_max == -1) ? iConsoleHeight : y_max;
	x_min = (x_min == -1) ? 0 : x_min;
	x_max = (x_max == -1) ? iConsoleWidth : x_max;

	min_y = (min_y < y_min) ? y_min : min_y;
	max_y = (max_y > y_max) ? y_max : max_y;


	for (int16_t y = min_y; y < max_y; y++)
	{
		for (size_t i = 0; i < points.size(); i++)
		{
			if ((edges[i].y1 >= y && edges[i].y2 < y) || (edges[i].y1 < y && edges[i].y2 >= y))
			{
				// Count X
				scanex.push_back(edges[i].x1 + edges[i].del_xy * (y - edges[i].y1));
			}

			// if edge == y
			else if (edges[i].y1 == y && edges[i].y2 == y)
			{
				scanex.push_back(edges[i].x1);
				scanex.push_back(edges[i].x2);
			}
		}

		if (scanex.size())
		{
			std::sort(scanex.begin(), scanex.end());

			for (size_t i = 0; i < scanex.size() - 1; i += 2)
			{
				int16_t x1, x2;
				x1 = (scanex[i] < x_min) ? x_min : scanex[i];
				x2 = (scanex[i + 1] > x_max) ? x_max : scanex[i + 1];

				DrawLineBresenham(x1, y, x2, y, sym, col);
				//DrawLineBresenham(scanex[i], y, scanex[i + 1], y, c, col);
			}

			scanex.clear();
		}
	}
}

void Graphics::ShadingPolygonsFloodFillRecursion(const std::vector<fPoint2D>& points, int16_t sym, int16_t col, int16_t col_edges)
{
	fPoint2D center;

	for (auto& point : points)
	{
		center += point;
	}
	center /= points.size();

	center.x = roundf(center.x);
	center.y = roundf(center.y);

	if (center.x <= 0.0f || center.x >= iConsoleWidth || center.y <= 0.0f || center.y >= iConsoleHeight)
	{
		fPoint2D new_center;
		int16_t counter = 0;
		for (auto& point : points)
		{
			if (point.x >= 0.0f && point.x <= iConsoleWidth && point.y >= 0.0f && point.y <= iConsoleHeight)
			{
				new_center += point;
				counter++;
			}
		}

		new_center += center;
		counter++;
		new_center /= counter;

		center = new_center;
	}

	if (center.x >= 0.0f && center.x <= iConsoleWidth && center.y >= 0.0f && center.y <= iConsoleHeight)
	{
		CHAR_INFO* console_ptr = &console[(int16_t)center.y * iConsoleWidth + (int16_t)center.x];;

		if (console_ptr->Attributes != col_edges && console_ptr->Attributes != col)
			FillingFloodFill(console_ptr, center.x, center.y, sym, col, col_edges);
	}
}

void Graphics::FillingFloodFill(CHAR_INFO* console_ptr, int16_t x, int16_t y, int16_t sym, int16_t col, int16_t col_edges)
{
	// lambda for checking edges of screen
	auto on_screen = [this](int16_t x, int16_t y)
	{
		if (x >= 0.0f && x <= iConsoleWidth && y >= 0.0f && y <= iConsoleHeight)
			return true;
		return false;
	};

	Draw(x, y, sym, col);

	console_ptr = &console[(int16_t)y * iConsoleWidth + (int16_t)x];

	if (on_screen(x, y - 1) && (console_ptr - iConsoleWidth)->Attributes != col_edges && (console_ptr - iConsoleWidth)->Attributes != col)
		FillingFloodFill(console_ptr, x, y - 1, sym, col, col_edges);
	if (on_screen(x, y + 1) && (console_ptr + iConsoleWidth)->Attributes != col_edges && (console_ptr + iConsoleWidth)->Attributes != col)
		FillingFloodFill(console_ptr, x, y + 1, sym, col, col_edges);
	if (on_screen(x - 1, y) && (console_ptr - 1)->Attributes != col_edges && (console_ptr - 1)->Attributes != col)
		FillingFloodFill(console_ptr, x - 1, y, sym, col, col_edges);
	if (on_screen(x + 1, y) && (console_ptr + 1)->Attributes != col_edges && (console_ptr + 1)->Attributes != col)
		FillingFloodFill(console_ptr, x + 1, y, sym, col, col_edges);
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

void Graphics::RotatePolygons(std::vector<fPoint2D>& points, float& angle)
{
	fPoint2D center;

	for (auto& point : points)
	{
		center += point;
	}

	center /= points.size();

	for (auto& point : points)
	{
		RotateLineAroundPoint(center.x, center.y, point.x, point.y, angle);
	}
}

bool Graphics::ScalingLine(fPoint2D& point1, fPoint2D& point2, float k)
{
	fPoint2D centers;

	// Center of the line
	centers = (point1 + point2) / 2;

	if ((point1.x >= 1 && point1.x <= GetConsoleWidth() - 1 && point1.y >= 1 && point1.y <= GetConsoleHeight() - 1)
		&& (point2.x >= 1 && point2.x <= GetConsoleWidth() - 1 && point2.y >= 1 && point2.y <= GetConsoleHeight() - 1) || k < 1.0f)
	{
		if ((fabs(point1.x - point2.x) > 0.01f
			&& fabs(point1.y - point2.y) > 0.01f) || k > 1.0f)
		{
			point1 = ((point1 - centers) * k) + centers;
			point2 = ((point2 - centers) * k) + centers;

			return false;
		}
	}

	return true;
}

bool Graphics::ScalingPolygons(std::vector<fPoint2D>& points, float k)
{
	fPoint2D center;									// Center coords
	fPoint2D near_point;

	near_point = points[0];

	for (auto& point : points)
	{
		center += point;
	}

	center /= points.size();

	int16_t count = 0;								// Number of visible points

	for (auto& point : points)
	{
		// Counting number of visible points
		if (point.x >= 1 && point.x <= GetConsoleWidth() - 1 && point.y >= 1 && point.y <= GetConsoleHeight() - 1)
			count++;

		// Find near point about center
		if (fabs(point.x - center.x) <= near_point.x && fabs(point.y - center.y) <= near_point.y)
			near_point = point;
	}

	if (((fabs(near_point.x - center.x) > 0.01f)
		&& (fabs(near_point.y - center.y) > 0.01f)) || k > 1.0f)
	{
		if (count == points.size() || k < 1.0f)
		{
			for (auto& point : points)
			{
				point = ((point - center) * k) + center;
			}

			return false;
		}
	}

	return true;
}

bool Graphics::crossingNumber(triangle tr, float _x, float _y)
{
	// This function determines number of intersections with polygon

	int16_t cn = 0;    // the  crossing number counter
	int16_t i = 0;

	do
	{
		int16_t next = (i + 1) % 3;

		if (((tr.points[i].y <= _y) && (tr.points[next].y > _y)) ||
			((tr.points[i].y > _y) && (tr.points[next].y <= _y)))
		{
			float vt = (_y - tr.points[i].y) / (tr.points[next].y - tr.points[i].y);
			if (_x < tr.points[i].x + vt * (tr.points[next].x - tr.points[i].x))
				++cn;
		}

		i = next;

	} while (i != 0);

	return (cn & 1);    // 0 if even (out), and 1 if  odd (in)
}

bool Graphics::onSegment(const fPoint3D& p, const fPoint3D& q, const fPoint3D& r)
{
	// Find intersections between two segments
		// https://www.youtube.com/watch?v=bbTqI0oqL5U&t=596s&ab_channel=TECHDOSE

	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;

	return false;
}

int16_t Graphics::orientation(const fPoint3D& p, const fPoint3D& q, const fPoint3D& r)
{
	// To find orientation of ordered triplet (p, q, r). 
	// The function returns following values 
	// 0 --> p, q and r are colinear 
	// 1 --> Clockwise 
	// 2 --> Counterclockwise 

	// See https://www.geeksforgeeks.org/orientation-3-ordered-points/ 
		// for details of below formula. 
	int16_t val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;			// colinear 

	return (val > 0) ? 1 : 2;		// clock or counterclock wise 
}

bool Graphics::doIntersect(const fPoint3D& p1, const fPoint3D& q1, const fPoint3D& p2, const fPoint3D& q2)
{
	// The main function that returns true if line segment 'p1q1' 
	// and 'p2q2' intersect. 

	// Find the four orientations needed for general and 
		// special cases 
	int16_t o1 = orientation(p1, q1, p2);
	int16_t o2 = orientation(p1, q1, q2);
	int16_t o3 = orientation(p2, q2, p1);
	int16_t o4 = orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1 
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases 
}

bool Graphics::checkPointAndSegment(const fPoint3D& start, const fPoint3D& p, const fPoint3D& end)
{
	float del_x = end.x - start.x;
	float del_y = end.y - start.y;
	float del_xy = (abs(del_y) <= 0.00001f) ? 0.0f : del_x / del_y;
	float del_yx = (abs(del_x) <= 0.00001f) ? 0.0f : del_y / del_x;

	if (onSegment(start, p, end))
	{
		if (abs(del_x) <= 0.00001f)
			return true;

		if (abs(del_y) <= 0.00001f)
			return true;

		if (start.x + del_xy * (p.y - start.y) - 1.0f <= p.x && start.x + del_xy * (p.y - start.y) + 1.0f >= p.x)
			if (start.y + del_yx * (p.x - start.x) - 1.0f <= p.y && start.y + del_yx * (p.x - start.x) + 1.0f >= p.y)
				return true;
	}

	return false;
}

void Graphics::WarnockAlgorithm(std::vector<triangle>& vecTrianglesToRaster, float _left_x,
	float _right_x, float _top_y, float _bottom_y)
{
	// We need to find relationships of our screen and polygons

	// For more info:
		// https://geomalgorithms.com/a03-_inclusion.html
		// https://stackoverflow.com/questions/27589796/check-point-within-polygon
	

	// lambda for checking edges of screen
	auto on_screen = [&](triangle tr)
	{

		if (crossingNumber(tr, _left_x, _top_y)) return true;
		if (crossingNumber(tr, _right_x, _top_y)) return true;
		if (crossingNumber(tr, _left_x, _bottom_y)) return true;
		if (crossingNumber(tr, _right_x, _bottom_y)) return true;

		for (int16_t i = 0; i < 3; i++)					// One point on screen or more
		{
			if (tr.points[i].x >= _left_x && tr.points[i].x <= _right_x && tr.points[i].y >= _top_y && tr.points[i].y <= _bottom_y)
				return true;
		}


		int16_t i = 0;
		fPoint3D p1(_left_x, _top_y, 0.0f);
		fPoint3D p2(_right_x, _top_y, 0.0f);
		fPoint3D p3(_left_x, _bottom_y, 0.0f);
		fPoint3D p4(_right_x, _bottom_y, 0.0f);

		do
		{
			int16_t next = (i + 1) % 3;

			if (doIntersect(tr.points[i], tr.points[next], p1, p2)) return true;
			if (doIntersect(tr.points[i], tr.points[next], p1, p3)) return true;
			if (doIntersect(tr.points[i], tr.points[next], p2, p4)) return true;
			if (doIntersect(tr.points[i], tr.points[next], p3, p4)) return true;

			i = next;
		} while (i != 0);


		return false;
	};

	// Show net
	/*DrawLineBresenham(_left_x, _top_y, _right_x, _top_y, PIXEL_SOLID, FG_GREY);
	DrawLineBresenham(_left_x, _top_y, _left_x, _bottom_y, PIXEL_SOLID, FG_GREY);
	DrawLineBresenham(_left_x, _bottom_y, _right_x, _bottom_y, PIXEL_SOLID, FG_GREY);
	DrawLineBresenham(_right_x, _top_y, _right_x, _bottom_y, PIXEL_SOLID, FG_GREY);*/


	// Delete all disjoin polygons
	std::vector<triangle> tempTriangles;

	for (auto& tri : vecTrianglesToRaster)
	{
		if (on_screen(tri))
			tempTriangles.push_back(tri);
	}

	// Main algorithm

	if (tempTriangles.empty())					// If all polygons were disjoined
		return;

	else if (tempTriangles.size() == 1)			// If only one polygon
	{
		std::vector<fPoint2D> points(3);
		for (int16_t i = 0; i < 3; i++)
		{
			points[i].x = tempTriangles.front().points[i].x;
			points[i].y = tempTriangles.front().points[i].y;
		}

		//ShadingPolygonsScanLine(points, PIXEL_SOLID, FG_BLUE, _top_y, _bottom_y, _left_x, _right_x);
	}
												// If we have many polygons
	else if (tempTriangles.size() > 1)
	{
		if ((_right_x - _left_x) > 1.0f)		// Check dx (if > 1 then we need to split or draw, if <= 1 then we have 1 pixel)
		{
			int16_t count_point = 0;

			if (crossingNumber(tempTriangles.front(), _left_x, _top_y)) count_point++;
			if (crossingNumber(tempTriangles.front(), _right_x, _top_y)) count_point++;
			if (crossingNumber(tempTriangles.front(), _left_x, _bottom_y)) count_point++;
			if (crossingNumber(tempTriangles.front(), _right_x, _bottom_y)) count_point++;

			if (count_point == 4)							// Surrounding polygon
			{
				//Fill(_left_x, _top_y, _right_x, _bottom_y, PIXEL_SOLID, FG_DARK_YELLOW);
				Fill(_left_x, _top_y, _right_x, _bottom_y, tempTriangles.front().sym, tempTriangles.front().col);
			}
															// Split into windows
			else
			{
				tempTriangles.clear();


				if ((_bottom_y - _top_y) > 1.0f)			// if <= 1 then we cant split Y anymore
				{
					float dx = (_right_x - _left_x) / 2;
					float dy = (_bottom_y - _top_y) / 2;


					// Top Left
					WarnockAlgorithm(vecTrianglesToRaster, _left_x, _left_x + roundf(dx), _top_y, _top_y + roundf(dy));

					// Top Right
					WarnockAlgorithm(vecTrianglesToRaster, _left_x + roundf(dx), _right_x, _top_y, _top_y + roundf(dy));

					// Bottom Left
					WarnockAlgorithm(vecTrianglesToRaster, _left_x, _left_x + roundf(dx), _top_y + roundf(dy), _bottom_y);

					// Bottom Right
					WarnockAlgorithm(vecTrianglesToRaster, _left_x + roundf(dx), _right_x, _top_y + roundf(dy), _bottom_y);
				}

				else									// split only X coords
				{
					float dx = (_right_x - _left_x) / 2;

					// Top Left
					WarnockAlgorithm(vecTrianglesToRaster, _left_x, _left_x + roundf(dx), _top_y, _top_y);

					// Top Right
					WarnockAlgorithm(vecTrianglesToRaster, _left_x + roundf(dx), _right_x, _top_y, _top_y);

					// Bottom Left
					WarnockAlgorithm(vecTrianglesToRaster, _left_x, _left_x + roundf(dx), _top_y, _bottom_y);

					// Bottom Right
					WarnockAlgorithm(vecTrianglesToRaster, _left_x + roundf(dx), _right_x, _top_y, _bottom_y);
				}
			}
		}

		else					// else we have i pixel cube then draw it
		{
			//Fill(_left_x, _top_y, _right_x, _bottom_y, PIXEL_SOLID, FG_RED);
			Fill(_left_x, _top_y, _right_x, _bottom_y, tempTriangles.front().sym, tempTriangles.front().col);
		}
	}
}

std::vector<Graphics::triangle> Graphics::RobertsAlgorithm(std::vector<triangle>& vecTrianglesToRaster, fPoint3D& view_point,
	fPoint3D& barycenter, int16_t sym, int16_t col, int16_t col_edge)
{
	fPoint3D vec1, vec2;
	std::vector<triangle> vecVisibleSurfaces;

	bool its_edge = false;

	for (auto& tri : vecTrianglesToRaster)
	{
		vec1 = tri.points[0] - tri.points[1];
		vec2 = tri.points[2] - tri.points[1];

		float d;
		fPoint3D v;
		v = Vector_CrossProduct(vec1, vec2);
		d = -Vector_DotProduct(v, tri.points[0]);

		if ((Vector_DotProduct(v, barycenter) + d) < 0.0f)
		{
			v *= -1.0f;;
			d *= -1.0f; 
		}

		if ((Vector_DotProduct(v, view_point) + d) < 0.0f)
		{
			// If its edge
			if (checkPointAndSegment(tri.points[0], tri.points[2], tri.points[1])) its_edge = true;
			else if (checkPointAndSegment(tri.points[0], tri.points[1], tri.points[2])) its_edge = true;
			else if (checkPointAndSegment(tri.points[1], tri.points[0], tri.points[2])) its_edge = true;


			if (!its_edge)
			{
				std::vector<fPoint2D> points(3);
				for (int16_t i = 0; i < 3; i++)
				{
					points[i].x = tri.points[i].x;
					points[i].y = tri.points[i].y;
				}

				ShadingPolygonsScanLine(points, sym, col);
				DrawPolygons(points, sym, col_edge);
				//WriteConsoleOutput(hConsole, console, { iConsoleWidth, iConsoleHeight }, { 0,0 }, &rectWindow);
				//ShadingPolygonsFloodFillRecursion(points, sym, col, col_edge);

				vecVisibleSurfaces.push_back(tri);
			}
			its_edge = false;
		}
	}

	return vecVisibleSurfaces;
}

void Graphics::PainterAlgorithm(std::vector<triangle>& vecTrianglesToRaster, int16_t sym, int16_t col, int16_t col_edge)
{
	std::vector<fPoint2D> lines(3);

	bool its_edge = false;
	for (auto& tri : vecTrianglesToRaster)
	{
		// If its edge
		if (checkPointAndSegment(tri.points[0], tri.points[2], tri.points[1])) its_edge = true;
		else if (checkPointAndSegment(tri.points[0], tri.points[1], tri.points[2])) its_edge = true;
		else if (checkPointAndSegment(tri.points[1], tri.points[0], tri.points[2])) its_edge = true;


		if (!its_edge)
		{
			for (int16_t i = 0; i < 3; i++)
			{
				lines[i].x = tri.points[i].x;
				lines[i].y = tri.points[i].y;
			}

			// Выводим на экран

			ShadingPolygonsScanLine(lines, sym, tri.col);

			//DrawPolygons(lines, sym, col_edge);
			//ShadingPolygonsFloodFillRecursion(lines, sym, tri.col, col_edge);
			DrawPolygons(lines, sym, tri.col);

			//WriteConsoleOutput(hConsole, console, { iConsoleWidth, iConsoleHeight }, { 0,0 }, &rectWindow);
		}
		its_edge = false;
	}
}

void Graphics::DrawShadow(std::vector<triangle>& vecTrianglesToRaster, fPoint3D& light)
{
	std::vector<triangle> vecShadow = vecTrianglesToRaster;

	for (auto& tri : vecShadow)
	{
		for (int16_t i = 0; i < 3; i++)
		{
			// Infinity light:

			/*tri.points[i].x -= light.x * (tri.points[i].y / light.y);
			tri.points[i].z -= light.z * (tri.points[i].y / light.y);
			tri.points[i].y = 0.30f * static_cast<float>(iConsoleHeight) + tri.points[i].z * 50.0f;*/


			// Local light:

			tri.points[i].x = tri.points[i].x + (0.50f * static_cast<float>(iConsoleHeight) - tri.points[i].y)
				* (tri.points[i].x - light.x) / (tri.points[i].y - light.y);

			tri.points[i].z = tri.points[i].z + (0.50f * static_cast<float>(iConsoleHeight) - tri.points[i].y) 
				* (tri.points[i].z - light.z) / (tri.points[i].y - light.y);

			tri.points[i].y = 0.70f * static_cast<float>(iConsoleHeight) - tri.points[i].z * 20.0f;
		}
	}

	std::vector<fPoint2D> lines(3);

	for (auto& tri : vecShadow)
	{
		for (int16_t i = 0; i < 3; i++)
		{
			lines[i].x = tri.points[i].x;
			lines[i].y = tri.points[i].y;
		}

		// Draw shadow
		ShadingPolygonsScanLine(lines, PIXEL_SOLID, FG_GREY);
	}
}

void Graphics::MoveTo2D(std::vector<fPoint2D>& points, mat3x3& m)
{
	for (auto& point : points)
	{
		point.x += m.m[1][0];
		point.y += m.m[0][1];
	}
}

void Graphics::GetBarycenter3D(std::vector<triangle>& vecTrianglesToRaster, fPoint3D& barycenter)
{
	std::vector<fPoint3D> points;
	bool equal = false;

	for (auto& tri : vecTrianglesToRaster)						// find all point (once)
	{
		for (size_t i = 0; i < 3; i++)
		{
			if (points.empty())
				points.push_back(tri.points[i]);
			else
			{
				for (size_t j = 0; j < points.size(); j++)
				{
					if (points[j] == tri.points[i])			// if not equal
					{
						equal = true;
						break;
					}
				}

				if (!equal)
				{
					points.push_back(tri.points[i]);	
				}
				equal = false;
			}
		}
	}

	for (auto& p : points)
	{
		barycenter += p;
	}
	barycenter /= points.size();
}

float Graphics::Vector_DotProduct(fPoint3D& v1, fPoint3D& v2)
{
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

float Graphics::Vector_Length(fPoint3D& v)
{
	return sqrt(Vector_DotProduct(v, v));;
}

Graphics::fPoint3D Graphics::Vector_Normalise(fPoint3D& v)
{
	float l = Vector_Length(v);
	return v / l;
}

Graphics::fPoint3D Graphics::Vector_CrossProduct(fPoint3D& v1, fPoint3D& v2)
{
	fPoint3D v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

Graphics::fPoint2D Graphics::MultiplyMatrixVector(mat3x3& m, fPoint2D& v)
{
	fPoint2D v1;

	v1.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.w * m.m[2][0];
	v1.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.w * m.m[2][1];
	v1.w = v.x * m.m[0][2] + v.y * m.m[1][2] + v.w * m.m[2][2];

	return v1;
}

Graphics::fPoint3D Graphics::MultiplyMatrixVector(mat4x4& m, fPoint3D& v)
{
	fPoint3D v1;

	v1.x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + v.w * m.m[3][0];
	v1.y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + v.w * m.m[3][1];
	v1.z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + v.w * m.m[3][2];
	v1.w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + v.w * m.m[3][3];

	return v1;
}

Graphics::mat4x4 Graphics::Matrix_MakeIdentity()
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

Graphics::mat4x4 Graphics::Matrix_MakeRotationX(float fAngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[1][2] = sinf(fAngleRad);
	matrix.m[2][1] = -sinf(fAngleRad);
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

Graphics::mat4x4 Graphics::Matrix_MakeRotationY(float fAngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][2] = sinf(fAngleRad);
	matrix.m[2][0] = -sinf(fAngleRad);
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

Graphics::mat4x4 Graphics::Matrix_MakeRotationZ(float fAngleRad)
{
	mat4x4 matrix;
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][1] = sinf(fAngleRad);
	matrix.m[1][0] = -sinf(fAngleRad);
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

Graphics::mat4x4 Graphics::Matrix_MakeScale(float x, float y, float z)
{
	mat4x4 matrix;
	matrix.m[0][0] = x;
	matrix.m[1][1] = y;
	matrix.m[2][2] = z;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

Graphics::mat4x4 Graphics::Matrix_MakeTranslation(float x, float y, float z)
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	matrix.m[3][0] = x;
	matrix.m[3][1] = y;
	matrix.m[3][2] = z;
	return matrix;
}

Graphics::mat4x4 Graphics::Matrix_MakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
	float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * PI);
	mat4x4 matrix;
	matrix.m[0][0] = fAspectRatio * fFovRad;
	matrix.m[1][1] = fFovRad;
	matrix.m[2][2] = fFar / (fFar - fNear);
	matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;
	return matrix;
}

Graphics::mat4x4 Graphics::Matrix_MultiplyMatrix(mat4x4& m1, mat4x4& m2)
{
	mat4x4 matrix;
	for (int16_t c = 0; c < 4; c++)
		for (int16_t r = 0; r < 4; r++)
			matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
	return matrix;
}