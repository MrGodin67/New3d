#include "PercompIncludes.h"
#include "Game.h"
#include "Direct3DWindow.h"


void Game::EndApp()
{
	PostQuitMessage(0);
}

Game::Game(Direct3DWindow & wnd)
	:
	gfx(wnd.ScreenWidth(),wnd.ScreenHeight(),wnd.WindowHandle(),
		wnd.m_vsync, wnd.m_fullscreen),
	m_cam(wnd.m_cam)
{
	wnd.SetGameObject(this);
}


HRESULT Game::OnUpdate(UpdateEventArgs & e)
{
	return S_OK;
}

HRESULT Game::OnRender(RenderEventArgs & e)
{
	return S_OK;
}

void Game::OnKeyPressed(KeyEventArgs & e)
{
	
}

void Game::OnKeyReleased(KeyEventArgs & e)
{
}

void Game::OnMouseMoved(MouseMotionEventArgs & e)
{
}

void Game::OnMouseButtonPressed(MouseButtonEventArgs & e)
{
}

void Game::OnMouseButtonReleased(MouseButtonEventArgs & e)
{
}

void Game::OnMouseWheel(MouseWheelEventArgs & e)
{
}

void Game::OnWndMessage(WndMsgEventArgs & e)
{
}
