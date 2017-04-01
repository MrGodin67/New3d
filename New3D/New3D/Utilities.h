#pragma once

#include "ShaderFactory.h"
#include <algorithm>
namespace Utilities
{
	const float D3DX_PI = 3.14159265358979323846f;
	class BoundingBox
	{
	public:
		DirectX::XMFLOAT3 center;
		DirectX::XMFLOAT3 minPt;
		DirectX::XMFLOAT3 maxPt;
		BoundingBox();
		BoundingBox(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 minPt, DirectX::XMFLOAT3 maxPt);
		static BoundingBox Make(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 minExt, DirectX::XMFLOAT3 maxExt)
		{
			BoundingBox b;
			b.center = center;
			b.minPt = DirectX::XMFLOAT3(center.x + minExt.x, center.y + minExt.y, center.z + minExt.z);
			b.maxPt = DirectX::XMFLOAT3(center.x + maxExt.x, center.y + maxExt.y, center.z + maxExt.z);
			return b;
		}
		BoundingBox TransformCoords(DirectX::XMMATRIX world);
	};
	class Plane
	{
	public:
		DirectX::XMFLOAT3 pt0;
		DirectX::XMFLOAT3 pt1;
		DirectX::XMFLOAT3 pt2;
		DirectX::XMFLOAT3 normal;
		int index;
		Plane();
		Plane(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2);
		void MakeNormal();
		// for debug
		BoundingBox box;
	};
	class Sphere
	{
	public:
		Sphere();
		Sphere(DirectX::XMFLOAT3 center, float radius);
		DirectX::XMFLOAT3 center;
		float radius;
	};
	class LineSegment
	{
	public:
		DirectX::XMFLOAT3 pt0;
		DirectX::XMFLOAT3 pt1;
		LineSegment() {}
		LineSegment(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1);
	};

	

	static inline float DegToRad(float angle) { return angle * D3DX_PI / 180.0f; }
	static inline float RadToDeg(float rad) { return rad * 180.0f / D3DX_PI; }
	static inline float GetAngleBetweenPoints2D(DirectX::XMFLOAT2 firstPoint, DirectX::XMFLOAT2 secondPoint) {

		if ((secondPoint.x > firstPoint.x)) {

			return (atan2((secondPoint.x - firstPoint.x), (firstPoint.y - secondPoint.y)) * 180.0f / D3DX_PI);

		}
		else if ((secondPoint.x < firstPoint.x)) {

			return 360.0f - (atan2((firstPoint.x - secondPoint.x), (firstPoint.y - secondPoint.y)) * 180.0f / D3DX_PI);

		}

		return atan2(0.0f, 0.0f);

	}
	static inline float GetRadiansBetweenPoints2D(DirectX::XMFLOAT2 firstPoint, DirectX::XMFLOAT2 secondPoint)
	{
		return DegToRad(GetAngleBetweenPoints2D(firstPoint, secondPoint));
	}
	
};

