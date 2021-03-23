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
			Draw(x, y, c, col);
			if (balance >= 0)
			{
				y += signY;
				balance -= deltaX;		// f(T) = f(M) - dx => half part f(T) [still need dy]
			}

			balance += deltaY;			// f(M) + dy => f(B) or part of f(T)
			x += signX;
		}

		Draw(x, y, c, col);
	}

	else								// Similarly for axis [Y>X]
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

void Graphics::DrawPolygons(std::vector<fPoint2D>& points, int16_t c, int16_t col)
{
	size_t i;

	for (i = 0; i < points.size() - 1; i++)
	{
		DrawLineBresenham(roundf(points[i].x), roundf(points[i].y), roundf(points[i + 1].x), roundf(points[i + 1].y), c, col);
	}
	DrawLineBresenham(roundf(points[i].x), roundf(points[i].y), roundf(points[0].x), roundf(points[0].y), c, col);
}

void Graphics::ClearConsole(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t c, int16_t col)
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

void Graphics::ShadingPolygonsScanLine(const std::vector<fPoint2D>& points, int16_t c, int16_t col, int16_t y_min, int16_t y_max,
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

				DrawLineBresenham(x1, y, x2, y, c, col);
				//DrawLineBresenham(scanex[i], y, scanex[i + 1], y, c, col);
			}

			scanex.clear();
		}
	}
}

void Graphics::ShadingPolygonsFloodFill(const std::vector<fPoint2D>& points, int16_t c, int16_t col, int16_t col_edges)
{
	// Its Beta version!
	// It has some problems with filling some corners

	fPoint2D center, temp;
	std::queue<fPoint2D> queue;

	for (auto& point : points)
	{
		center += point;
	}
	center /= points.size();
	
	center.x = roundf(center.x);
	center.y = roundf(center.y);

	queue.push(std::move(center));

	CHAR_INFO* console_ptr = nullptr;


	Draw(center.x, center.y, c, col);

	// lambda for checking edges of screen
	auto on_screen = [this](int16_t x, int16_t y)
	{
		if (x >= 0.0f && x <= GetConsoleWidth() && y >= 0.0f && y <= GetConsoleHeight())
			return true;
		return false;
	};

	while (!queue.empty())
	{
		temp = std::move(queue.front());
		queue.pop();
		
		console_ptr = &console[(int16_t)temp.y * iConsoleWidth + (int16_t)temp.x];

		// Top
		if ((console_ptr - iConsoleWidth)->Attributes != col_edges && (console_ptr - iConsoleWidth)->Attributes != col 
			&& on_screen(temp.x, temp.y - 1.0f))
		{
			queue.push(std::move(fPoint2D(temp.x, temp.y - 1.0f)));
			Draw(temp.x, temp.y - 1.0f, c, col);
		}

		// Bottom
		if ((console_ptr + iConsoleWidth)->Attributes != col_edges && (console_ptr + iConsoleWidth)->Attributes != col
			&& on_screen(temp.x, temp.y + 1.0f))
		{
			queue.push(std::move(fPoint2D(temp.x, temp.y + 1.0f)));
			Draw(temp.x, temp.y + 1.0f, c, col);
		}

		// Left
		if ((console_ptr - 1)->Attributes != col_edges && (console_ptr - 1)->Attributes != col
			&& on_screen(temp.x - 1.0f, temp.y))
		{
			queue.push(std::move(fPoint2D(temp.x - 1.0f, temp.y)));
			Draw(temp.x - 1.0f, temp.y, c, col);
		}

		// Right
		if ((console_ptr + 1)->Attributes != col_edges && (console_ptr + 1)->Attributes != col
			&& on_screen(temp.x + 1.0f, temp.y))
		{
			queue.push(std::move(fPoint2D(temp.x + 1.0f, temp.y)));
			Draw(temp.x + 1.0f, temp.y, c, col);
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

void Graphics::WarnockAlgorithm(std::vector<triangle>& vecTrianglesToRaster, fPoint3D tl_corner,
	fPoint3D tr_corner, fPoint3D bl_corner, fPoint3D br_corner)
{
	// We need to find relationships of our screen and polygons

	// For more info:
		// https://geomalgorithms.com/a03-_inclusion.html
		// https://stackoverflow.com/questions/27589796/check-point-within-polygon
// This function determines number of intersections with polygon
	auto crossing_number = [&](triangle tr, fPoint3D point)
	{
		int16_t cn = 0;    // the  crossing number counter
		int16_t i = 0;

		do
		{
			int16_t next = (i + 1) % 3;

			if ((tr.p[i].y <= point.y && (tr.p[next].y > point.y)) || 
				((tr.p[i].y > point.y) && (tr.p[next].y <= point.y)))
			{
				float vt = (point.y - tr.p[i].y) / (tr.p[next].y - tr.p[i].y);
				if (point.x < tr.p[i].x + vt * (tr.p[next].x - tr.p[i].x))
					++cn;
			}

			i = next;

		} while (i != 0);

		return (cn & 1);    // 0 if even (out), and 1 if  odd (in)
	};


// Find intersections between two segments
	// https://www.youtube.com/watch?v=bbTqI0oqL5U&t=596s&ab_channel=TECHDOSE
	auto onSegment = [&](fPoint3D p, fPoint3D q, fPoint3D r)
	{
		if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
			q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
			return true;

		return false;
	};

	// To find orientation of ordered triplet (p, q, r). 
	// The function returns following values 
	// 0 --> p, q and r are colinear 
	// 1 --> Clockwise 
	// 2 --> Counterclockwise 
	auto orientation = [&](fPoint3D p, fPoint3D q, fPoint3D r)
	{
		// See https://www.geeksforgeeks.org/orientation-3-ordered-points/ 
		// for details of below formula. 
		int16_t val = (q.y - p.y) * (r.x - q.x) -
			(q.x - p.x) * (r.y - q.y);

		if (val == 0) return 0;			// colinear 

		return (val > 0) ? 1 : 2;		// clock or counterclock wise 
	};

	// The main function that returns true if line segment 'p1q1' 
	// and 'p2q2' intersect. 
	auto doIntersect = [&](fPoint3D p1, fPoint3D q1, fPoint3D p2, fPoint3D q2)
	{
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
	};

// lambda for checking edges of screen
	auto on_screen = [&](triangle tr)
	{
		int16_t count_point = 0;

		if (crossing_number(tr, tl_corner)) count_point++;
		if (crossing_number(tr, tr_corner)) count_point++;
		if (crossing_number(tr, bl_corner)) count_point++;
		if (crossing_number(tr, br_corner)) count_point++;

		if (count_point == 4)							// Surrounding polygon
			return true;

		for (int16_t i = 0; i < 3; i++)					// One point on screen or more
		{
			if (tr.p[i].x >= tl_corner.x && tr.p[i].x <= tr_corner.x && tr.p[i].y >= tl_corner.y && tr.p[i].y <= bl_corner.y)
				return true;
		}


		int16_t i = 0;

		do
		{
			int16_t next = (i + 1) % 3;

			if (doIntersect(tr.p[i], tr.p[next], tl_corner, tr_corner)) return true;

			if (doIntersect(tr.p[i], tr.p[next], tl_corner, bl_corner)) return true;

			if (doIntersect(tr.p[i], tr.p[next], bl_corner, br_corner)) return true;

			if (doIntersect(tr.p[i], tr.p[next], tr_corner, br_corner)) return true;

			i = next;
		} while (i!=0);
		

		return false;
	};

	// Show net
	//DrawLineBresenham(tl_corner.x, tl_corner.y, tr_corner.x, tr_corner.y, PIXEL_SOLID, FG_GREY);
	//DrawLineBresenham(tl_corner.x, tl_corner.y, bl_corner.x, bl_corner.y, PIXEL_SOLID, FG_GREY);
	//DrawLineBresenham(tr_corner.x, tr_corner.y, br_corner.x, br_corner.y, PIXEL_SOLID, FG_GREY);
	//DrawLineBresenham(bl_corner.x, bl_corner.y, br_corner.x, br_corner.y, PIXEL_SOLID, FG_GREY);


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
			points[i].x = tempTriangles.front().p[i].x;
			points[i].y = tempTriangles.front().p[i].y;
		}

		ShadingPolygonsScanLine(points, PIXEL_SOLID, FG_BLUE, tl_corner.y, bl_corner.y, tl_corner.x, tr_corner.x);
	}
	else if (tempTriangles.size() > 1 && (tr_corner.x - tl_corner.x) > 1.0f && (bl_corner.y - tl_corner.y) > 1.0f)
	{
		int16_t count_point = 0;

		if (crossing_number(tempTriangles.front(), tl_corner)) count_point++;
		if (crossing_number(tempTriangles.front(), tr_corner)) count_point++;
		if (crossing_number(tempTriangles.front(), bl_corner)) count_point++;
		if (crossing_number(tempTriangles.front(), br_corner)) count_point++;

		if (count_point == 4)							// Surrounding polygon
		{
			ClearConsole((int16_t)tl_corner.x, (int16_t)tl_corner.y, (int16_t)br_corner.x, (int16_t)br_corner.y, PIXEL_SOLID, FG_BLUE);
		}
														// Split into windows
		else
		{
			tempTriangles.clear();

			// Top Left
			WarnockAlgorithm(vecTrianglesToRaster, tl_corner, fPoint3D(tl_corner.x + roundf((tr_corner.x - tl_corner.x) / 2), tr_corner.y, 0.0f),
				fPoint3D(bl_corner.x, tl_corner.y + roundf((bl_corner.y - tl_corner.y) / 2), 0.0f),
				fPoint3D(tl_corner.x + roundf((tr_corner.x - tl_corner.x) / 2), tl_corner.y + roundf((bl_corner.y - tl_corner.y) / 2), 0.0f));
			// Top Right
			WarnockAlgorithm(vecTrianglesToRaster, fPoint3D(tl_corner.x + roundf((tr_corner.x - tl_corner.x) / 2), tl_corner.y, 0.0f), tr_corner,
				fPoint3D(tl_corner.x + roundf((tr_corner.x - tl_corner.x) / 2), tl_corner.y + roundf((bl_corner.y - tl_corner.y) / 2), 0.0f),
				fPoint3D(br_corner.x, tl_corner.y + roundf((bl_corner.y - tl_corner.y) / 2), 0.0f));
			// Bottom Left
			WarnockAlgorithm(vecTrianglesToRaster, fPoint3D(tl_corner.x, tl_corner.y + roundf((bl_corner.y - tl_corner.y) / 2), 0.0f),
				fPoint3D(tl_corner.x + roundf((tr_corner.x - tl_corner.x) / 2), tl_corner.y + roundf((bl_corner.y - tl_corner.y) / 2), 0.0f),
				bl_corner, fPoint3D(tl_corner.x + roundf((tr_corner.x - tl_corner.x) / 2), br_corner.y, 0.0f));
			// Bottom Right
			WarnockAlgorithm(vecTrianglesToRaster, fPoint3D(tl_corner.x + roundf((tr_corner.x - tl_corner.x) / 2), tl_corner.y + roundf((bl_corner.y - tl_corner.y) / 2), 0.0f),
				fPoint3D(tr_corner.x, tl_corner.y + roundf((bl_corner.y - tl_corner.y) / 2), 0.0f),
				fPoint3D(tl_corner.x + roundf((tr_corner.x - tl_corner.x) / 2), br_corner.y, 0.0f), br_corner);
		}
	}
	else
	{
		// Need to rework this part

		std::vector<triangle> t;

		t.push_back(std::move(tempTriangles.front()));
		tempTriangles.erase(tempTriangles.begin());

		float tz = t[0].p[0].z + t[0].p[1].z + t[0].p[2].z;

		float _z = 0.0f;
		for (auto& tri : tempTriangles)
		{
			_z = tri.p[0].z + tri.p[1].z + tri.p[2].z;
			if (tz == _z)
				t.push_back(std::move(tri));
		}

		for (auto& tri : t)
		{
			std::vector<fPoint2D> points(3);
			for (int16_t i = 0; i < 3; i++)
			{
				points[i].x = tri.p[i].x;
				points[i].y = tri.p[i].y;
			}

			
			ShadingPolygonsScanLine(points, PIXEL_SOLID, FG_BLUE, tl_corner.y, bl_corner.y, tl_corner.x, tr_corner.x);
			//WriteConsoleOutput(hConsole, console, { iConsoleWidth, iConsoleHeight }, { 0,0 }, & rectWindow);
			//DrawPolygons(points, PIXEL_SOLID, FG_MAGENTA);
		}
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