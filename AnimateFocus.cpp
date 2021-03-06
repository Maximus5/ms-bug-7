
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "resource.h"

HINSTANCE hInst;
const wchar_t gszWindowClass[] = L"AnimateFocusTest";
const wchar_t gszTitle[] = L"AnimateFocus Test";
const wchar_t gszMessage[] = L"Press 'Minimize' button to animate the window";

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

// Main function for bug demonstration
void DoAnimateWindow(HWND hWnd)
{
	DWORD nFlags = AW_SLIDE | AW_VER_NEGATIVE | AW_HIDE;
	DWORD nDuration = 2000/*ms*/;
	
	// We expect here hiding window with animation from bottom to top
	::AnimateWindow(hWnd, nDuration, nFlags);

	// Bug#1. Window is hidden, but still has focus, seems like there is no easy way
	// to emulate standard window minimization

	// Bug#2. If the window has WS_THICKFRAME the animation looks ugly, because
	// only *contents* of the window is animated, but the frame (non-client area)
	// stays in place.

	// Just show window icon on the TaskBar to restore it later
	// This line does not affect the bug, nothing changes if we comment it
	::ShowWindow(hWnd, SW_SHOWMINNOACTIVE);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ANIMATEFOCUS));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ANIMATEFOCUS);
	wcex.lpszClassName = gszWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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

	POINT ptCur{}; GetCursorPos(&ptCur);
	HMONITOR hMon = MonitorFromPoint(ptCur, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = {sizeof(mi)}; GetMonitorInfo(hMon, &mi);

	const auto& rcWork = mi.rcWork;
	const auto width = rcWork.right - rcWork.left;
	const auto height = rcWork.bottom - rcWork.top;

	HWND hWnd = CreateWindowW(gszWindowClass, gszTitle, WS_OVERLAPPEDWINDOW,
		rcWork.left + width/3, rcWork.top, width/3, height/3,
		nullptr, nullptr, hInstance, nullptr);

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
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
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
		int state = SaveDC(hdc);
		RECT rcClient{}; GetClientRect(hWnd, &rcClient);
		SelectObject(hdc, (HFONT)GetStockObject(DEFAULT_GUI_FONT));
		DrawText(hdc, gszMessage, -1, &rcClient, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		RestoreDC(hdc, state);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_SYSCOMMAND:
		if (wParam == SC_MINIMIZE)
		{
			DoAnimateWindow(hWnd);
			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
