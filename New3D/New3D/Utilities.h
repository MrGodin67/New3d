#pragma once

#include "ShaderFactory.h"
#include <algorithm>
namespace Utilities
{
	using namespace DirectX;
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

	inline float GetYawAToB(DirectX::XMFLOAT3* A, DirectX::XMFLOAT3* B)
	{
		return atan2f((B->x - A->x), (B->z - A->z));
	};
	inline float GetPitchAToB(DirectX::XMFLOAT3* A, DirectX::XMFLOAT3* B)
	{
		DirectX::XMFLOAT3 v;
		v.x = A->x - B->x;
		v.y = A->y - B->y;
		v.z = A->z - B->z;
		float len = sqrt(v.x * v.x + v.z * v.z);
		return(atanf(v.y / len));
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
	static inline void MapMouseToWorld(DirectX::XMVECTOR& outOrigin, DirectX::XMVECTOR& outDir,
		DirectX::XMMATRIX matProj, DirectX::XMMATRIX matView, DirectX::XMFLOAT2 screen, DirectX::XMFLOAT2 mouse)
	{

		
		DirectX::XMFLOAT4X4 m_matProj;
		DirectX::XMStoreFloat4x4(&m_matProj, matProj);

		// Compute the vector of the pick ray in screen space
		DirectX::XMFLOAT3 v;
		v.x = (((2.0f * mouse.x) / screen.x) - 1) / m_matProj._11;
		v.y = -(((2.0f * mouse.y) / screen.y) - 1) / m_matProj._22;
		v.z = 1.0f;

		DirectX::XMFLOAT4X4 m;
		DirectX::XMStoreFloat4x4(&m, XMMatrixInverse(0, matView));
		
		outDir = DirectX::XMVectorSet(v.x*m._11 + v.y*m._21 + v.z*m._31,
			v.x*m._12 + v.y*m._22 + v.z*m._32,
			v.x*m._13 + v.y*m._23 + v.z*m._33, 1.0f);

		outDir = DirectX::XMVector3Normalize(outDir);

		outOrigin = DirectX::XMVectorSet(m._41, m._42, m._43, 1.0f);
		outOrigin += outDir*0.01f;




	}
	static inline void WorldToScreen(D3D11_VIEWPORT & vp, DirectX::XMMATRIX & world, DirectX::XMMATRIX & view, DirectX::XMMATRIX & proj,
		DirectX::XMVECTOR & outVector, float & outDist)
	{

		DirectX::XMVECTOR pos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMVECTOR objPt = DirectX::XMVector3TransformCoord(pos, world);
		DirectX::XMVECTOR camPt = DirectX::XMVector3TransformCoord(pos, XMMatrixInverse(0, view));
		outDist = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(objPt - camPt));
		outVector = DirectX::XMVector3Project(pos, vp.TopLeftX, vp.TopLeftY,
			vp.Width, vp.Height, vp.MinDepth, vp.MaxDepth,
			proj, view, world);


	}

	static inline DirectX::XMFLOAT2 ConvertTo3DScreenSpace(DirectX::XMFLOAT2 screenSize,DirectX::XMFLOAT2& pos)
	{
		float X = (float)((screenSize.x / 2) * -1) + pos.x;
		float Y = (float)(screenSize.y / 2) - pos.y;
		return DirectX::XMFLOAT2(X, Y);
	}
	static inline float norm(float value, float min, float max) {
		return (value - min) / (max - min);
	};

	static inline float lerp(float norm, float min, float max) {
		return (max - min) * norm + min;
	};

	static inline float map(float value, float sourceMin, float sourceMax, float destMin, float destMax) {
		return lerp(norm(value, sourceMin, sourceMax), destMin, destMax);
	};
};

