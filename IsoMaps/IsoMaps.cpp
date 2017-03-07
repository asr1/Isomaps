// IsoMaps.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib, "Gdiplus.lib")
#include "IsoMaps.h"
#include <list>
#include <string>
#include <ctime>
#include "Line.h"

#define MAX_LOADSTRING 100
#define MIN_ZOOM 50
#define MAX_ZOOM 500
#define CLOSE_ENOUGH_OFFSET 50

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
RECT rect;										// pointer to the size of the window
HWND hWnd;										// current window
int resizeOffset = 100;							//Changed when zooming
bool outdated = false;							//Keep zoom up to date
std::list<Line> lines;	
HWND hWndExample;								//Global text handler
bool lineIntersect(POINT p);					//Checks to see if a Line is clicked


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
Gdiplus::Color getColorAtLocation(POINT p);
void changeOffset(int offset);
void addAllLines();
Gdiplus::Color getRandomColor();
VOID onPaint(HDC hdc);
bool areCloseEnough(int a, int b);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	srand((int)time(0));

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_ISOMAPS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ISOMAPS));

    MSG msg;

	addAllLines();


    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	Gdiplus::GdiplusShutdown(gdiplusToken);

    return (int) msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ISOMAPS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_ISOMAPS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   SCROLLINFO inf;
   inf.fMask = SIF_ALL;
   inf.nMin = 0;
   inf.nMax = 100;
   inf.nPos = 0;
   inf.nPage = 5;


   GetScrollInfo(hWnd, SB_VERT, &inf);
   GetScrollInfo(hWnd, SB_HORZ, &inf);


   if (!hWnd)
   {
      return FALSE;
   }

   //AddText
   hWndExample = CreateWindow(L"STATIC", L"Text Goes Here", WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 10, 100, 100, hWnd, NULL, hInstance, NULL);



   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
// WM_MOUSEWHEEL - Check scroll wheel for zoom or scroll
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	short zDelta;
	short fwKeys;

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
			onPaint(hdc);
            EndPaint(hWnd, &ps);
			return 0;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_MOUSEWHEEL:
		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		fwKeys = GET_KEYSTATE_WPARAM(wParam);
		switch (fwKeys)
		{
		case MK_CONTROL:
			changeOffset(zDelta);
			break;
		case MK_SHIFT:
			ScrollWindowEx(hWnd, zDelta, 0, NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN);
			break;
		default:
			ScrollWindowEx(hWnd, 0, zDelta, NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN);
			break;
		}
		InvalidateRect(hWnd, NULL, true);
		break;
	case WM_LBUTTONUP:
		// Get the current cursor position
		POINT p;
		
		if (GetCursorPos(&p))
		{
			getColorAtLocation(p);
		}
			
		
		break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void changeOffset(int offset)
{
	resizeOffset += offset;
	if (resizeOffset < MIN_ZOOM)
	{
		resizeOffset = MIN_ZOOM;
	}
	if (resizeOffset > MAX_ZOOM)
	{
		resizeOffset = MAX_ZOOM;
	}

	outdated = true;
}

void addAllLines()
{

	for (int i = 0; i * resizeOffset < rect.bottom; i++)
	{
		for (int j = 0; j * resizeOffset < rect.right; j++)
		{
			Line line = Line(10 + j * resizeOffset, i * resizeOffset, 0, resizeOffset, DEFAULT_THICKNESS, VERT);
			line.multx = i;
			line.multy = j;
			lines.push_back(line);
		}
	}

	for (int i = 0; i * resizeOffset < rect.bottom; i++)
	{
		for (int j = 0; j * resizeOffset < rect.right; j++)
		{
			Line line = Line(j * resizeOffset, 10 + i * resizeOffset, resizeOffset, 0, DEFAULT_THICKNESS, HORZ);
			line.multx = i;
			line.multy = j;
			lines.push_back(line);
		}
	}
	InvalidateRect(hWnd, NULL, true);
}

Gdiplus::Color getRandomColor()
{
	return Gdiplus::Color(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1);
}

VOID onPaint(HDC hdc)
{
	GetWindowRect(hWnd, &rect);
	Gdiplus::Graphics graphics(hdc);

	if (outdated)
	{
		lines.clear();
		addAllLines();//Refresh zoom size
		outdated = false;
	}

	//Paint all lines
	for (std::list<Line>::iterator it = lines.begin(); it != lines.end(); it++)
	{
		Line l = *it;
		Gdiplus::Pen pen(l.color, l.thickness);
		graphics.DrawLine(&pen, l.x, l.y, l.x + l.width, l.y + l.height);
	}
}



Gdiplus::Color getColorAtLocation(POINT p)
{
	GetUpdateRect(hWnd, &rect, true);
	ScreenToClient(hWnd, &p);





	COLORREF color;
	HDC hDC;
	BOOL b;

	// Get the device context for the screen
	hDC = GetDC(NULL);

	// Retrieve the color at that position
	color = GetPixel(hDC, p.x, p.y);


	// Release the device context again
	ReleaseDC(GetDesktopWindow(), hDC);

	char buffer[255];

	wchar_t wtext[255];
	//sprintf(buffer, "%i %i %i", GetRValue(color), GetGValue(color), GetBValue(color));
	//sprintf(buffer, "%d %d", p.x - rect.left, p.y - rect.top);
	if (lineIntersect(p))
	{
		sprintf(buffer, "True");
	}
	else
	{
		sprintf(buffer, "False");
	}


	//DEBUG
	char temp = ' ';

	if (areCloseEnough(p.x, p.y))
	{
		temp = 't';
	}
	else
	{
		temp = 'f';
	}

	sprintf(buffer, "%d %d %c", p.x, p.y, temp);
	
	//DEBUG


	mbstowcs(wtext, buffer, strlen(buffer) + 1);
	SetWindowText(hWndExample, wtext);
	return Gdiplus::Color(GetRValue(color), GetGValue(color), GetBValue(color));
}

bool lineIntersect(POINT p)
{
	for (std::list<Line>::iterator it = lines.begin(); it != lines.end(); it++)
	{	
		if (areCloseEnough(p.x, (*it).x) && p.y >= (*it).y && p.y <= (*it).y + (*it).height ||
			areCloseEnough(p.y, (*it).y) && p.x >= (*it).x && p.x <= (*it).x + (*it).width)
		{
			return true;
		}
		//
		//if (p.x == (*it).x && p.y >= (*it).y && p.y <= (*it).y + (*it).height ||
		//	p.y == (*it).y && p.x >= (*it).x && p.x <= (*it).x + (*it).width) {
		//	return true;
		//}
	}
	return false;
}

bool areCloseEnough(int a, int b)
{
	return abs(a - b) < CLOSE_ENOUGH_OFFSET;
}