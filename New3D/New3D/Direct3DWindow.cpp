
#include "PercompIncludes.h"
#include "Direct3DWindow.h"
#include "game.h"
#include <windows.h>

Direct3DWindow::Direct3DWindow()
{
	
}

Direct3DWindow::Direct3DWindow(int scrnWidth, int scrnHeight, LPCWSTR appName, bool v_sync, bool fullscreen)
	:m_screenWidth(scrnWidth),m_screenHeight(scrnHeight),
	m_fullscreen(fullscreen),m_applicationName(appName),m_vsync(v_sync)
{
	InitializeWindows();
	
}


Direct3DWindow::Direct3DWindow(const Direct3DWindow& other)

{
}


Direct3DWindow::~Direct3DWindow()
{
}





void Direct3DWindow::Shutdown()
{
	// Release the application wrapper object.
	

	// Shutdown the window.
	ShutdownWindows();

	return;
}


bool Direct3DWindow::ProcessMessage()
{
	MSG msg;
	bool ok;
	HRESULT hr = S_OK;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	static float totalTime = 0.0f;
	// Loop until there is a quit message from the window or the user.
	ok = true;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
		{
			ok = false;
		}
	}

	
	UpdateEventArgs updateEventArgs(0.016f, 0.0f);
	RenderEventArgs renderEventArgs(0.016f, 0.0f);
	
	if (FAILED(m_pGame->OnUpdate(updateEventArgs)))
		return false;
	if(FAILED(m_pGame->OnRender(renderEventArgs)))
		return false;
	return ok;
}

int Direct3DWindow::ScreenWidth()
{
	return m_screenWidth;
}

int Direct3DWindow::ScreenHeight()
{
	return m_screenHeight;
}

void Direct3DWindow::ScreenHalfExtents(float & x, float & y)
{
	
	x = (float)( m_screenWidth / 2);
	y = (float)( m_screenHeight / 2);
	
}

HWND & Direct3DWindow::WindowHandle()
{
	return m_hwnd;
}



MouseButtonEventArgs::MouseButton DecodeMouseButton(UINT messageID)
{
	MouseButtonEventArgs::MouseButton mouseButton = MouseButtonEventArgs::None;
	switch (messageID)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	{
		mouseButton = MouseButtonEventArgs::Left;
	}
	break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	{
		mouseButton = MouseButtonEventArgs::Right;
	}
	break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	{
		mouseButton = MouseButtonEventArgs::Middel;
	}
	break;
	}

	return mouseButton;
}
LRESULT CALLBACK Direct3DWindow::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	
	m_cam.HandleMessages(hwnd, umsg, wparam, lparam);
	switch (umsg)
	{
	case WM_KEYDOWN:
	{
		MSG charMsg;
		// Get the unicode character (UTF-16)
		unsigned int c = 0;
		// For printable characters, the next message will be WM_CHAR.
		// This message contains the character code we need to send the KeyPressed event.
		// Inspired by the SDL 1.2 implementation.
		if (PeekMessage(&charMsg, hwnd, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR)
		{
			GetMessage(&charMsg, hwnd, 0, 0);
			c = charMsg.wParam;
		}
		bool shift = GetAsyncKeyState(VK_SHIFT) > 0;
		bool control = GetAsyncKeyState(VK_CONTROL) > 0;
		bool alt = GetAsyncKeyState(VK_MENU) > 0;
		KeyCode::Key key = (KeyCode::Key)wparam;
		unsigned int scanCode = (lparam & 0x00FF0000) >> 16;
		KeyEventArgs keyEventArgs(key, c, KeyEventArgs::Pressed, shift, control, alt);
		m_pGame->OnKeyPressed(keyEventArgs);
	}
	break;
	case WM_KEYUP:
	{
		bool shift = GetAsyncKeyState(VK_SHIFT) > 0;
		bool control = GetAsyncKeyState(VK_CONTROL) > 0;
		bool alt = GetAsyncKeyState(VK_MENU) > 0;
		KeyCode::Key key = (KeyCode::Key)wparam;
		unsigned int c = 0;
		unsigned int scanCode = (lparam & 0x00FF0000) >> 16;

		// Determine which key was released by converting the key code and the scan code
		// to a printable character (if possible).
		// Inspired by the SDL 1.2 implementation.
		unsigned char keyboardState[256];
		GetKeyboardState(keyboardState);
		wchar_t translatedCharacters[4];
		if (int result = ToUnicodeEx(wparam, scanCode, keyboardState, translatedCharacters, 4, 0, NULL) > 0)
		{
			c = translatedCharacters[0];
		}

		KeyEventArgs keyEventArgs(key, c, KeyEventArgs::Released, shift, control, alt);
		m_pGame->OnKeyReleased(keyEventArgs);
	}
	break;
	case WM_MOUSEMOVE:
	{
		bool lButton = (wparam & MK_LBUTTON) != 0;
		bool rButton = (wparam & MK_RBUTTON) != 0;
		bool mButton = (wparam & MK_MBUTTON) != 0;
		bool shift = (wparam & MK_SHIFT) != 0;
		bool control = (wparam & MK_CONTROL) != 0;

		int x = ((int)(short)LOWORD(lparam));
		int y = ((int)(short)HIWORD(lparam));

		MouseMotionEventArgs mouseMotionEventArgs(lButton, mButton, rButton, control, shift, x, y);
		m_pGame->OnMouseMoved(mouseMotionEventArgs);
	}
	break;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	{
		bool lButton = (wparam & MK_LBUTTON) != 0;
		bool rButton = (wparam & MK_RBUTTON) != 0;
		bool mButton = (wparam & MK_MBUTTON) != 0;
		bool shift = (wparam & MK_SHIFT) != 0;
		bool control = (wparam & MK_CONTROL) != 0;

		int x = ((int)(short)LOWORD(lparam));
		int y = ((int)(short)HIWORD(lparam));
		POINT clientToScreenPoint;
		clientToScreenPoint.x = x;
		clientToScreenPoint.y = y;
		ScreenToClient(hwnd, &clientToScreenPoint);
		MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(umsg), MouseButtonEventArgs::Pressed, lButton, mButton, rButton, control, shift, (int)clientToScreenPoint.x, (int)clientToScreenPoint.y);
		m_pGame->OnMouseButtonPressed(mouseButtonEventArgs);
	}
	break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	{
		bool lButton = (wparam & MK_LBUTTON) != 0;
		bool rButton = (wparam & MK_RBUTTON) != 0;
		bool mButton = (wparam & MK_MBUTTON) != 0;
		bool shift = (wparam & MK_SHIFT) != 0;
		bool control = (wparam & MK_CONTROL) != 0;

		int x = ((int)(short)LOWORD(lparam));
		int y = ((int)(short)HIWORD(lparam));

		MouseButtonEventArgs mouseButtonEventArgs(DecodeMouseButton(umsg), MouseButtonEventArgs::Released, lButton, mButton, rButton, control, shift, x, y);
		m_pGame->OnMouseButtonReleased(mouseButtonEventArgs);
	}
	break;
	case WM_MOUSEWHEEL:
	{
		// The distance the mouse wheel is rotated.
		// A positive value indicates the wheel was rotated to the right.
		// A negative value indicates the wheel was rotated to the left.
		float zDelta = ((int)(short)HIWORD(wparam)) / (float)WHEEL_DELTA;
		short keyStates = (short)LOWORD(wparam);

		bool lButton = (keyStates & MK_LBUTTON) != 0;
		bool rButton = (keyStates & MK_RBUTTON) != 0;
		bool mButton = (keyStates & MK_MBUTTON) != 0;
		bool shift = (keyStates & MK_SHIFT) != 0;
		bool control = (keyStates & MK_CONTROL) != 0;

		int x = ((int)(short)LOWORD(lparam));
		int y = ((int)(short)HIWORD(lparam));

		// Convert the screen coordinates to client coordinates.
		POINT clientToScreenPoint;
		clientToScreenPoint.x = x;
		clientToScreenPoint.y = y;
		ScreenToClient(hwnd, &clientToScreenPoint);

		MouseWheelEventArgs mouseWheelEventArgs(zDelta, lButton, mButton, rButton, control, shift, (int)clientToScreenPoint.x, (int)clientToScreenPoint.y);
		m_pGame->OnMouseWheel(mouseWheelEventArgs);
	}
	break;
	
	}
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}


void Direct3DWindow::InitializeWindows()
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	;

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	int screenWidth2 = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight2 = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (m_fullscreen)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth2;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight2;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
		m_screenWidth = screenWidth2;
		m_screenHeight = screenHeight2;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - m_screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - m_screenHeight) / 2;
	}
#define WS_D3DWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX)

	RECT rect;
	rect.left = posX;
	rect.top = posY;
	rect.right = posX + m_screenWidth;
	rect.bottom = posY + m_screenHeight;
	AdjustWindowRect(&rect, WS_EX_APPWINDOW, true);
	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, WS_D3DWINDOW,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	
	ShowCursor(true);

	return;
}


void Direct3DWindow::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (m_fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

void Direct3DWindow::SetGameObject(Game * game)
{
	m_pGame = game;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	
	
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
	return 0;
}
