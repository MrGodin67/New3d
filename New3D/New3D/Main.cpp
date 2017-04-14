#include "PercompIncludes.h"
#include <windows.h>
#include "Direct3DWindow.h"
#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Direct3DWindow window(800, 600, L"test app", true, false);
	Application app(window);
	
	
	while (window.ProcessMessage())
	{	};
	window.Shutdown();
	return 0;
}