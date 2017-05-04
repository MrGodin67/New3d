#pragma once
#include "Camera3D_2.h"
#include <DirectXMath.h>
struct PlayerAmmo
{
	DirectX::XMVECTOR pos;
	DirectX::XMVECTOR vel;
	DirectX::XMVECTOR collisionPt;
	DirectX::XMFLOAT3 mat_scale = {1.0f,1.0f,1.0f};
	DirectX::XMMATRIX mat_rot;
	float speed;
	float lifeSpan = 1.0f;
	float timer = 0.0f;
	bool active = true;
	float rotation = 0.0f;
	DirectX::XMMATRIX GetMatrix();
	void update(float dt, DirectX::XMFLOAT3 playerPos);
};

struct Gun
{
	DirectX::XMVECTOR pos;
	DirectX::XMVECTOR vel;
	
	float speed = 10.0f;
	float sideOffset;
	float timer = 0.0f;
	float shotDelay;
	bool canShoot = true;
	Gun(float side_offset)
		:sideOffset(side_offset)
	{}
	void update(float dt,DirectX::XMMATRIX mat)
	{
		(timer += dt) > shotDelay ? canShoot = true : canShoot = false;
		pos = XMVector3TransformCoord(DirectX::XMVectorSet(sideOffset, 0.0f, 0.15f, 1.0f), XMMatrixInverse(&DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), mat));
	}
	void setTarget(DirectX::XMVECTOR targetPos)
	{
		
		 vel = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(pos, targetPos));
	}
	bool fireShot(PlayerAmmo& ammo)
	{
		if (canShoot)
		{
			ammo.lifeSpan = 3.0f;
			ammo.pos = pos;
			ammo.vel = vel;
			ammo.speed = speed;
			ammo.active = true;
			ammo.mat_scale = { 0.1f,0.1f,0.1f };
			timer = 0.0f;
			return true;
		}
		return false;
	}
};
class Player
{
private:
	CFirstPersonCamera& m_cam;
	
	float m_heightOffTerrain = 0.25f;
	Gun m_leftGun;
	Gun m_rightGun;
	int leftShots = 0;
	int rightShots = 0;
	float timer = 0.0f;
	float gunShotInterval = 1.0f;
	bool flipShot = false;
public:
	Player(CFirstPersonCamera& cam)
		:m_cam(cam),m_leftGun(-0.15f),m_rightGun(0.15f)
	{
		m_leftGun.shotDelay = m_rightGun.shotDelay = 0.75f;
	}
	void Update(float dt, class Terrain* terrain,bool doShot);
	void SetHeight(float height);
	void RenderDebugStats(class Graphics& gfx, size_t n_ammo);
	PlayerAmmo FireShot(DirectX::XMVECTOR target);
	
};