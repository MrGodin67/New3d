#pragma once
#include "Graphics.h"
#include "Events.h"
#include "Camera3D_2.h"
class Game
{
protected:
	Graphics gfx;
	void EndApp();
	CFirstPersonCamera& m_cam;
	enum GAME_STATE
	{
		menu,
		running,
		paused
	};
public:
	Game(class Direct3DWindow& wnd);
	virtual ~Game() {}
	virtual HRESULT OnUpdate(UpdateEventArgs& e);
	virtual HRESULT OnRender(RenderEventArgs& e);
	virtual void OnKeyPressed(KeyEventArgs& e);
	virtual void OnKeyReleased(KeyEventArgs& e);
	virtual void OnMouseMoved(MouseMotionEventArgs& e);
	virtual void OnMouseButtonPressed(MouseButtonEventArgs& e);
	virtual void OnMouseButtonReleased(MouseButtonEventArgs& e);
	virtual void OnMouseWheel(MouseWheelEventArgs& e);
	virtual void OnWndMessage(WndMsgEventArgs& e);
	////////////////////////////////////
	// LOGIC
	////////////////////////////////////

private:
	
};