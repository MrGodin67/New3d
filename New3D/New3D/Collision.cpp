#include "PercompIncludes.h"
#include "Collision.h"

using namespace DirectX;
bool Collision::LineIntersectPlane(const Utilities::LineSegment & line, const Utilities::Plane plane, DirectX::XMFLOAT3 & intersectPt,float & distance)
{
	bool hit = false;
	XMVECTOR linePt1 = XMLoadFloat3(&line.pt0);
	XMVECTOR linePt2 = XMLoadFloat3(&line.pt1);
	XMVECTOR planePt0 = XMLoadFloat3(&plane.pt0);
	XMVECTOR planePt1 = XMLoadFloat3(&plane.pt1);
	XMVECTOR planePt2 = XMLoadFloat3(&plane.pt2);
	XMVECTOR normal = XMLoadFloat3(&plane.normal);

	// Find distance from linePt1 and linePt2 to the plane defined by the triangle
	XMVECTOR D1 = linePt1 - planePt0;
	XMVECTOR D2 = linePt2 - planePt0;
	
	XMVECTOR result;
	result = XMVector3Dot(D1, normal);
	float Dist1 = XMVectorGetX(result);
	result = XMVector3Dot(D2, normal);
	float Dist2 = XMVectorGetX(result);
	if ((Dist1 * Dist2) > 0.0f) 
		return false;  // line doesn't cross the triangle.
	if (Dist1 == Dist2) 
		return false;// line and plane are parallel

	// Find point on the line that intersects with the plane
	XMVECTOR IntersectPos = linePt1 + (linePt2 - linePt1) * (-Dist1 / (Dist2 - Dist1));

	// Find if the interesection point lies inside the triangle by testing it against all edges
	XMVECTOR vTest = XMVector3Cross(normal, planePt1 - planePt0);
	if (XMVectorGetX(XMVector3Dot(vTest, IntersectPos - planePt0)) < 0.0f)return false;
	
	 vTest = XMVector3Cross(normal, planePt2 - planePt1);
	if (XMVectorGetX(XMVector3Dot(vTest, IntersectPos - planePt1)) < 0.0f)return false;

	 vTest = XMVector3Cross(normal, planePt0 - planePt2);
	if (XMVectorGetX(XMVector3Dot(vTest, IntersectPos - planePt0)) < 0.0f)return false;


	distance = XMVectorGetX(XMVector3Length(linePt2 - IntersectPos));
	XMStoreFloat3(&intersectPt, IntersectPos);
	
	return true;
}

bool Collision::SphereToPlane( Utilities::Sphere & sphere,  Utilities::Plane & plane)
{
		//Calculate a vector from the point on the plane to the center of the sphere
	    XMVECTOR plane_center = (XMLoadFloat3(&plane.pt0) +XMLoadFloat3(&plane.pt1) + XMLoadFloat3(&plane.pt2)) / 3.0f;
		XMVECTOR sphere_center = XMLoadFloat3(&sphere.center);
		XMVECTOR vecTemp =sphere_center - plane_center;
		//vecTemp = XMVector3Normalize(vecTemp);
		XMVECTOR plane_normal = XMLoadFloat3(&plane.normal);
		//Calculate the distance: dot product of the new vector with the plane's normal
		float fDist = XMVectorGetX(XMVector3Dot(vecTemp, plane_normal));

		if (abs(fDist) > sphere.radius)
		{
			//The sphere is not touching the plane
			return false;
		}

		//Else, the sphere is colliding with the plane
		return true;
	
}

bool Collision::AABBToPlane(const Utilities::BoundingBox & box, const Utilities::Plane plane, DirectX::XMFLOAT3 & intersectPt, float & distance)
{
	XMFLOAT3 pt0;
	XMFLOAT3 pt1;
	XMFLOAT3 pt2;
	XMFLOAT3 pt3;
	XMFLOAT3 pt4;
	XMFLOAT3 pt5;
	XMFLOAT3 pt6;
	XMFLOAT3 pt7;
	float w = box.maxPt.x - box.minPt.x;
	float h = box.maxPt.y - box.minPt.y;
	float d = box.maxPt.z - box.minPt.z;
	pt0 = box.minPt;
	pt1 = box.minPt; pt1.y += h;
	pt2 = box.minPt; pt2.y += h; pt2.x += w;
	pt3 = box.minPt; pt3.x += w;

	pt4 = box.minPt; pt4.z += d;
	pt5 = box.minPt; pt5.y += h; pt5.z += d;
	pt6 = box.minPt; pt6.y += h; pt6.x += w; pt6.z += d;
	pt7 = box.minPt; pt7.x += w; pt7.z += d;
	Utilities::LineSegment lines[12];
	lines[0] = Utilities::LineSegment(pt0, pt1);
	lines[1] = Utilities::LineSegment(pt1, pt2);
	lines[2] = Utilities::LineSegment(pt2, pt3);
	lines[3] = Utilities::LineSegment(pt3, pt0);

	lines[4] = Utilities::LineSegment(pt4, pt5);
	lines[5] = Utilities::LineSegment(pt5, pt6);
	lines[6] = Utilities::LineSegment(pt6, pt7);
	lines[7] = Utilities::LineSegment(pt7, pt4);

	lines[8] = Utilities::LineSegment(pt0, pt4);
	lines[9] = Utilities::LineSegment(pt1, pt5);
	lines[10] = Utilities::LineSegment(pt2, pt6);
	lines[11] = Utilities::LineSegment(pt3, pt7);
	

	for (int c = 0; c < 12; c++)
	{
		if (LineIntersectPlane(lines[c], plane, intersectPt, distance))
		{
			return true;
		}
	}



	return false;
}

bool Collision::AABBtoAABB(const Utilities::BoundingBox & A, const Utilities::BoundingBox & B)
{
	return(A.maxPt.x > B.minPt.x &&
		A.minPt.x < B.maxPt.x &&
		A.maxPt.y > B.minPt.y &&
		A.minPt.y < B.maxPt.y &&
		A.maxPt.z > B.minPt.z &&
		A.minPt.z < B.maxPt.z);
}

Collision::Collision()
{
}


Collision::~Collision()
{
}
