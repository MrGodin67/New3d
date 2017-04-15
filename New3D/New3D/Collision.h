#pragma once
#include "Utilities.h"
class Collision
{
public:
	static bool LineIntersectPlane(const Utilities::LineSegment& line, const Utilities::Plane, DirectX::XMFLOAT3& intersectPt,float& distance);
	static bool SphereToPlane(Utilities::Sphere& sphere,  Utilities::Plane& plane);
	static bool AABBToPlane(const Utilities::BoundingBox& box, const Utilities::Plane, DirectX::XMFLOAT3& intersectPt, float& distance);
	static bool AABBtoAABB(const Utilities::BoundingBox& A, const Utilities::BoundingBox& B);
	Collision();
	~Collision();
};

