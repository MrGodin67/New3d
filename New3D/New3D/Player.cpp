#include "PercompIncludes.h"
#include "Player.h"
#include "Terrain.h"
#include "Graphics.h"
using namespace DirectX;
void Player::Update(float dt, Terrain * terrain)
{
	DirectX::XMVECTOR p = m_cam.GetEyePt();
	float height;
	terrain->GetHeight(p, height);
	p.m128_f32[1] = height + m_heightOffTerrain;
	m_cam.SetEye(DirectX::XMVectorGetX(p), DirectX::XMVectorGetY(p), DirectX::XMVectorGetZ(p));
	m_leftGun.update(dt,m_cam.GetViewMatrix());
	m_rightGun.update(dt,m_cam.GetViewMatrix());
	if (flipShot && timer == 0.0f)
		leftShots++;
	else if (!flipShot && timer == 0.0f)
		rightShots++;
	if ((timer += dt) > gunShotInterval)
	{
		flipShot = !flipShot;
		timer = 0.0f;
	}
	
}

void Player::SetHeight(float height)
{
	m_heightOffTerrain = height;
}

void Player::RenderDebugStats(Graphics & gfx)
{
	
	std::wstring text = L"shots Left (" + std::to_wstring(leftShots) + L") Right (" + std::to_wstring(rightShots) + L")";
	gfx.D2D_DrawFilledRectangle(nullptr, D2D1::RectF(10.0f, 10.0f, 350.0f, 60.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.5f));
	gfx.D2D_RenderText(FontFactory::FontDesc(text, gfx.D2DFonts()->GetFormat("Tahoma12"),
		D2D1::RectF(15.0f, 15.0f, 350.0f, 20.0f), D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)));
	
	text = L"Camera X (" + std::to_wstring(m_cam.GetEyePt2().x) + L") : Y : (" + std::to_wstring(m_cam.GetEyePt2().y) + L") Z : (" + std::to_wstring(m_cam.GetEyePt2().z) + L")";
	gfx.D2D_RenderText(FontFactory::FontDesc(text, gfx.D2DFonts()->GetFormat("Tahoma12"),
		D2D1::RectF(15.0f, 32.0f, 550.0f, 42.0f), D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)));
}

void PlayerAmmo::update(float dt)
{
	if ((timer += dt) > lifeSpan)
	{
		active = false;
	}
	else
	{
		pos = DirectX::XMVectorAdd(pos, vel) * speed * dt;
	}
}
