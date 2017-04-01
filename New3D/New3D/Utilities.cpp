#include "PercompIncludes.h"
#include "Utilities.h"
#include "Locator.h"
using namespace DirectX;
#define NOMINMAX
namespace Utilities
{

	BoundingBox::BoundingBox()
	{
		minPt = { FLT_MAX,FLT_MAX,FLT_MAX };
		maxPt = { -FLT_MAX,-FLT_MAX,-FLT_MAX };
	}

	BoundingBox::BoundingBox(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 minPt, DirectX::XMFLOAT3 maxPt)
		:center(center), minPt(minPt), maxPt(maxPt)
	{
	}

	BoundingBox BoundingBox::TransformCoords(DirectX::XMMATRIX world)
	{

		BoundingBox box;
		XMStoreFloat3(&box.center, XMVector3TransformCoord(XMLoadFloat3(&center), world));
		XMStoreFloat3(&box.minPt, XMVector3TransformCoord(XMLoadFloat3(&minPt), world));
		XMStoreFloat3(&box.maxPt, XMVector3TransformCoord(XMLoadFloat3(&maxPt), world));
		return box;
	}
	Utilities::Plane::Plane()
	{
	}

	Utilities::Plane::Plane(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2)
		:pt0(p0), pt1(p1), pt2(p2)
	{
		box.minPt.x = std::min<float>(std::min<float>(pt0.x, pt1.x), pt2.x);
		box.minPt.y = std::min<float>(std::min<float>(pt0.y, pt1.y), pt2.y);
		box.minPt.z = std::min<float>(std::min<float>(pt0.z, pt1.z), pt2.z);

		box.maxPt.x = std::max<float>(std::max<float>(pt0.x, pt1.x), pt2.x);
		box.maxPt.y = std::max<float>(std::max<float>(pt0.y, pt1.y), pt2.y);
		box.maxPt.z = std::max<float>(std::max<float>(pt0.z, pt1.z), pt2.z);
		DirectX::XMVECTOR center = DirectX::XMLoadFloat3(&pt0) - DirectX::XMLoadFloat3(&pt1) -
			DirectX::XMLoadFloat3(&pt2) / 3.0f;
		DirectX::XMStoreFloat3(&box.center, center);
	}

	void Utilities::Plane::MakeNormal()
	{
		XMVECTOR p1 = XMLoadFloat3(&pt1) - XMLoadFloat3(&pt0);
		XMVECTOR p2 = XMLoadFloat3(&pt2) - XMLoadFloat3(&pt0);
		XMVECTOR norm;
		norm = XMVector3Cross(p1, p2);
		norm = XMVector3Normalize(norm);
		XMStoreFloat3(&normal, norm);
	}
	Sphere::Sphere()
	{
	}
	Sphere::Sphere(DirectX::XMFLOAT3 center, float radius)
		:center(center),radius(radius)
	{
	}
	Utilities::LineSegment::LineSegment(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1)
		: pt0(p0), pt1(p1)
	{}

	
}
