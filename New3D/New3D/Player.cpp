#include "PercompIncludes.h"
#include "Player.h"
#include "Terrain.h"
#include "Graphics.h"
using namespace DirectX;
void Player::Update(float dt, Terrain * terrain,bool doShot)
{
	DirectX::XMVECTOR p = m_cam.GetEyePt();
	float height;
	terrain->GetHeight(p, height);
	p.m128_f32[1] = height + m_heightOffTerrain;
	m_cam.SetEye(DirectX::XMVectorGetX(p), DirectX::XMVectorGetY(p), DirectX::XMVectorGetZ(p));
	m_leftGun.update(dt,m_cam.GetViewMatrix());
	m_rightGun.update(dt,m_cam.GetViewMatrix());
	
	
	
}

void Player::SetHeight(float height)
{
	m_heightOffTerrain = height;
}

void Player::RenderDebugStats(Graphics & gfx, size_t n_ammo)
{
	
	std::wstring text = L"shots Left (" + std::to_wstring(leftShots) + L") Right (" + std::to_wstring(rightShots) + L")";
	gfx.D2D_DrawFilledRectangle(nullptr, D2D1::RectF(10.0f, 10.0f, 350.0f, 60.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.5f));
	gfx.D2D_RenderText(FontFactory::FontDesc(text, gfx.D2DFonts()->GetFormat("Tahoma12"),
		D2D1::RectF(15.0f, 15.0f, 350.0f, 20.0f), D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)));
	
	text = L"Camera X (" + std::to_wstring(m_cam.GetEyePt2().x) + L") : Y : (" + std::to_wstring(m_cam.GetEyePt2().y) + L") Z : (" + std::to_wstring(m_cam.GetEyePt2().z) + L")";
	gfx.D2D_RenderText(FontFactory::FontDesc(text, gfx.D2DFonts()->GetFormat("Tahoma12"),
		D2D1::RectF(15.0f, 32.0f, 550.0f, 42.0f), D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)));

	text = L"Ammo :" + std::to_wstring(n_ammo);
	gfx.D2D_RenderText(FontFactory::FontDesc(text, gfx.D2DFonts()->GetFormat("Tahoma12"),
		D2D1::RectF(15.0f, 56.0f, 550.0f, 42.0f), D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)));
}

PlayerAmmo Player::FireShot(DirectX::XMVECTOR target)
{
	PlayerAmmo ammo;
	ammo.active = false;
	flipShot = !flipShot;
	if (flipShot )
	{
		    ammo.lifeSpan = 8.0f;
		    ammo.pos = m_leftGun.pos;
			ammo.speed = m_leftGun.speed;
			ammo.active = true;
			ammo.collisionPt = target;
			ammo.vel = XMVector3Normalize( target - ammo.pos);
			XMFLOAT3 A, B;
			XMStoreFloat3(&A, ammo.pos);
			XMStoreFloat3(&B, target);
			float yaw = Utilities::GetYawAToB(&A, &B);
			float pitch = Utilities::GetPitchAToB(&A, &B);
			ammo.mat_rot = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
		
	}
	else if (!flipShot )
	{
		    ammo.lifeSpan = 8.0f;
		    ammo.pos = m_rightGun.pos;
			ammo.active = true;
			ammo.speed = m_rightGun.speed;
			ammo.collisionPt = target;
			ammo.vel = XMVector3Normalize(target - ammo.pos);
			XMFLOAT3 A, B;
			XMStoreFloat3(&A, ammo.pos);
			XMStoreFloat3(&B, target);
			float yaw = Utilities::GetYawAToB(&A, &B);
			float pitch = Utilities::GetPitchAToB(&A, &B);
			ammo.mat_rot = XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f);
		
		
	}
	ammo.mat_scale = { 0.1f,0.1f,0.1f };
	return ammo;
}

DirectX::XMMATRIX PlayerAmmo::GetMatrix()
{
	return XMMatrixScaling(mat_scale.x,mat_scale.y,mat_scale.z) *
		mat_rot* 
		XMMatrixTranslation(XMVectorGetX(pos), XMVectorGetY(pos), XMVectorGetZ(pos));
}

void PlayerAmmo::update(float dt,DirectX::XMFLOAT3 playerPos)
{
	if ((timer += dt) > lifeSpan)
	{
		active = false;
	}
	else
	{
		
		if (DirectX::XMVectorGetX(DirectX::XMVector3Length(pos - collisionPt)) < speed * dt)
			pos = collisionPt;
		else
			pos += vel * speed * dt;
		//XMFLOAT3 p;
		//XMStoreFloat3(&p, pos);
		//float angle = atan2(p.x - playerPos.x, p.z - playerPos.z) * (180.0f / XM_PI);
		//float rotation = angle * 0.0174532925f;
	}
	

}
