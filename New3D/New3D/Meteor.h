#pragma once

#include "Collision.h"

class Meteor
{
private:
	Utilities::Sphere m_collisionSphere;
	DirectX::XMFLOAT3 m_velocity;
	float m_acceleration = 0.0f;
	float m_mass = 1.0f;
public:
	Meteor(DirectX::XMFLOAT3 initialPos, DirectX::XMFLOAT3 initialVel,float speed);
	DirectX::XMVECTOR getPosition();
	void setPosition(DirectX::XMVECTOR& pos);
	DirectX::XMVECTOR getVelocity();
	void setVelocity(DirectX::XMVECTOR& vel);
	float getMass();
	void update(float dt);
};