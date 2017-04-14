#pragma once
#include "Game.h"
#include "terrainclass.h"
#include <memory>
#include "colorshaderclass.h"
#include "Camera3D_2.h"
#include "ShaderFactory.h"
#include "ColorLightShader.h"

static class Application* app;
class Application :
	public Game
{
	std::unique_ptr<TerrainClass> m_terrain;
	std::unique_ptr<ColorShaderClass> m_shader;
	std::unique_ptr<ShaderFactory::ShaderManager> m_shaders;
private:
	
	
	FontFactory::FontDesc txtDesc;
public:
	bool InitTerrain(char* fn);
	void InitShaders();
	
public:
	Application(class Direct3DWindow& wnd);
	~Application();
	virtual HRESULT OnUpdate(UpdateEventArgs& e);
	virtual HRESULT OnRender(RenderEventArgs& e);
	virtual void OnKeyPressed(KeyEventArgs& e);
	virtual void OnKeyReleased(KeyEventArgs& e);
	virtual void OnMouseMoved(MouseMotionEventArgs& e);
	virtual void OnMouseButtonPressed(MouseButtonEventArgs& e);
	virtual void OnMouseButtonReleased(MouseButtonEventArgs& e);
	virtual void OnMouseWheel(MouseWheelEventArgs& e);
	virtual void OnWndMessage(WndMsgEventArgs& e);
};

