#include "PercompIncludes.h"
#include "Meteor.h"
using namespace DirectX;
Meteor::Meteor(DirectX::XMFLOAT3 initialPos, DirectX::XMFLOAT3 initialVel, float speed)
  : m_velocity(initialVel),m_acceleration(speed)
{
	m_collisionSphere.center = initialPos;
	
}

DirectX::XMVECTOR Meteor::getPosition()
{
	return XMLoadFloat3(&m_collisionSphere.center);
}

DirectX::XMVECTOR Meteor::getVelocity()
{
	return XMLoadFloat3(&m_velocity);
}

void Meteor::setVelocity(DirectX::XMVECTOR & vel)
{
	XMStoreFloat3(&m_velocity, vel);
}
void Meteor::setPosition(DirectX::XMVECTOR & pos)
{
	XMStoreFloat3(&m_collisionSphere.center, pos);
}
float Meteor::getMass()
{
	return m_mass;
}

void Meteor::update(float dt)
{
	XMVECTOR p, v;
	p = XMLoadFloat3(&m_collisionSphere.center);
	v = XMLoadFloat3(&m_velocity);
	p += v * m_acceleration * dt;
	setPosition(p);
}
