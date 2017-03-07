// IsoMaps.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib, "Gdiplus.lib")
#include "IsoMaps.h"
#include <ctime>
#include "Line.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
RECT rect;										// pointer to the size of the window
HWND hWnd;										// current window
int resizeOffset = 100;							//Changed when zooming

struct node {
	Line data;
	node * next;
	node(const Line & dat, node * n = 0) : data(dat), next(n){}
	node(node * n = 0) : next(n) {}
};

node * head;
node * vertHead;
node * horzHead;

void insertAtEnd(node* &first, Line data) {
	// create node
	node* temp = new node;
	temp->data = data;
	temp->next = NULL;

	if (!first) { // empty list becomes the new node
		first = temp;
		return;
	}
	else { // find last and link the new node
		node* last = first;
		while (last->next) last = last->next;
		last->next = temp;
	}
	InvalidateRect(hWnd, NULL, true);
}

void changeOffset(int offset)
{
	resizeOffset += offset;
	if (resizeOffset < 10)
	{
		resizeOffset = 10;
	}
}

void addAllLines()
{

	for (int i = 0; i  * 10 < rect.bottom; i++)
	{
		for (int j = 0; j * 10 < rect.right; j++) 
		{
			Line line = Line(10, 0, 1, 1, DEFAULT_THICKNESS, HORZ);
			line.multx = j;
			line.multy = i;
			insertAtEnd(head, line);
		}
	}

	for (int i = 0; i * 10 < rect.bottom; i++)
	{
		for (int j = 0; j * 10 < rect.right; j++)
		{
			Line line = Line(0, 10, 0, 0, DEFAULT_THICKNESS, VERT);
			line.multx = j;
			line.multy = i;
			insertAtEnd(head, line);
		}
	}
}

void DeleteAllNodes(node * head)
{
	node * pointer = head->next;
	node * temp;
	while(pointer != nullptr)
	{
		temp = pointer->next;
		delete[] pointer;
		pointer = temp;
	}
}

Gdiplus::Color getRandomColor()
{
	return Gdiplus::Color(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1);
}

VOID onPaint(HDC hdc)
{
	GetWindowRect(hWnd, &rect);

	Gdiplus::Graphics graphics(hdc);


//	DeleteAllNodes(vertHead);
//	DeleteAllNodes(horzHead);

	Gdiplus::Pen p(Gdiplus::Color(255, 2550, 0), 3);

	//Sort the nodes
	for (node * curr = head; curr != nullptr; curr = curr->next)
	{
		Line l = curr->data;
		if (l.type == VERT) {
			insertAtEnd(vertHead, l);
		}
		else if (l.type == HORZ) {
			insertAtEnd(horzHead, l);
		}
	}

	if (horzHead->next != nullptr) {
		Line l = horzHead->next->next->data;
		Gdiplus::Pen pen(getRandomColor(), l.thickness);
		l.x = l.x + l.multx * resizeOffset;
		l.y = l.y + l.multy * resizeOffset;
		l.width = l.x + l.multx * resizeOffset;
		l.height = l.y + l.multy * resizeOffset;
		graphics.DrawLine(&pen, l.x, l.y, l.width, l.height);
	}


	//Paint all lines
	//for (node * curr = horzHead; curr != nullptr; curr = curr->next)
	//{
	//	Line l = curr->data;
	//	Gdiplus::Pen pen(getRandomColor(), l.thickness);
	//	graphics.DrawLine(&pen, l.x + l.multx * resizeOffset, l.y + l.multy * resizeOffset, l.x + l.multx * resizeOffset, l.y + l.multy * resizeOffset);
	//	//graphics.DrawLine(&pen, l.x + l.multx * resizeOffset, l.y + l.multy * resizeOffset, l.x + l.width, l.y + l.height + resizeOffset);
	//}

	///////reference
//
//for (int i = 0; i * resizeOffset < rect.bottom; i++)
//{
//	for (int j = 0; j * resizeOffset < rect.right; j++)
//	{
//		Line line = Line(10 + j * resizeOffset, i * resizeOffset, 0, resizeOffset, DEFAULT_THICKNESS, HORZ);
//		insertAtEnd(head, line);
//	}
//}
//
//for (int i = 0; i * resizeOffset < rect.bottom; i++)
//{
//	for (int j = 0; j * resizeOffset < rect.right; j++)
//	{
//		Line line = Line(j * resizeOffset, 10 + i * resizeOffset, resizeOffset, 0, DEFAULT_THICKNESS, VERT);
//		insertAtEnd(head, line);
//	}
//}

	//
	//
	//
	//for (int i = 0; i * offset < rect.bottom; i++)
	//{
	//	graphics.DrawLine(&pen, 0, i * offset, rect.right, i * offset);
	//}
	//for (int i = 0; i * offset < rect.right; i++)
	//{
	//	graphics.DrawLine(&pen, i * offset, 0, i * offset, rect.right);
	//}

	
}


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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
	head = new node;
	horzHead = new node;
	vertHead = new node;


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
