#pragma once
#include <windows.h>

#include <WindowsX.h>
#include "Events.h"
#include "Camera3D_2.h"
#include "Timer.h"

class Direct3DWindow
{
	friend class Game;
public:
	Direct3DWindow();
	Direct3DWindow(int scrnWidth, int scrnHeight, LPCWSTR appName,bool v_sync,bool fullscreen);
	Direct3DWindow(const Direct3DWindow&);
	~Direct3DWindow();
	
	
	void Shutdown();
	bool ProcessMessage();
	int ScreenWidth();
	int ScreenHeight();
	
	void ScreenHalfExtents(float& x, float& y);
	HWND& WindowHandle();
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	void InitializeWindows();
	void ShutdownWindows();
	void SetGameObject(Game* game);
	Timer m_timer;
	
private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	int m_screenHeight;
	int m_screenWidth;
	bool m_fullscreen;
	bool m_vsync;
	Game* m_pGame = nullptr;
	CFirstPersonCamera m_cam;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static Direct3DWindow* ApplicationHandle = 0;
