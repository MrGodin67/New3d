#pragma once
#include "Game.h"

#include <memory>
#include "Camera3D_2.h"
#include "Terrain.h"
#include "Player.h"
#include "Ammo.h"
#include "TextureFactory.h"
#include "Model2.h"
#include "DebugInfo.h"
#include "Retical.h"
#include "MyString.h"
class Application :
	public Game
{
	std::unique_ptr<Terrain> m_terrain;
    std::unique_ptr<Player> m_player;
	std::unique_ptr<Retical> m_retical;
	std::unordered_map<std::string, ID3D11ShaderResourceView*> m_textures;
	Model2 model;
	std::vector<PlayerAmmo> m_ammo;
	double terrainTime;
	Timer m_timer;
	Timer m_shotTimer;
private:
	bool fire_buttonDown = false;
	DirectX::XMVECTOR target;
	bool InitTerrain();
	FontFactory::FontDesc txtDesc;
	MenuViewPort debugDisplay;
public:
	
	
	
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

