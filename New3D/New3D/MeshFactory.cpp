#include "PercompIncludes.h"
#include "MeshFactory.h"
using namespace DirectX;
#include <vector>
std::unique_ptr<MeshFactory::Mesh<Geometry::LOD_VertexMin>> MeshFactory::MakeCubePTN(float size)
{	
		const int FaceCount = 6;

		static const DirectX::XMVECTORF32 faceNormals[FaceCount] =
		{
			{ 0,  0,  1 },
			{ 0,  0, -1 },
			{ 1,  0,  0 },
			{ -1,  0,  0 },
			{ 0,  1,  0 },
			{ 0, -1,  0 },
		};

		static const  DirectX::XMVECTORF32 textureCoordinates[4] =
		{
			{ 1, 0 },
			{ 1, 1 },
			{ 0, 1 },
			{ 0, 0 },
		};

		std::vector<Geometry::LOD_VertexMin> vertices;
		std::vector<DWORD> indices;

		size /= 2;

		// Create each face in turn.
		for (int i = 0; i < FaceCount; i++)
		{
			DirectX::XMVECTOR normal = faceNormals[i];

			// Get two vectors perpendicular both to the face normal and to each other.
			DirectX::XMVECTOR basis = (i >= 4) ? DirectX::g_XMIdentityR2 : DirectX::g_XMIdentityR1;

			DirectX::XMVECTOR side1 = DirectX::XMVector3Cross(normal, basis);
			DirectX::XMVECTOR side2 = DirectX::XMVector3Cross(normal, side1);

			// Six indices (two triangles) per face.
			DWORD vbase = (DWORD)vertices.size();
			indices.push_back(vbase + 0);
			indices.push_back(vbase + 1);
			indices.push_back(vbase + 2);

			indices.push_back(vbase + 0);
			indices.push_back(vbase + 2);
			indices.push_back(vbase + 3);
			Geometry::LOD_VertexMin vert;
			// Four vertices per face.
			DirectX::XMFLOAT3 pos, norm;
			DirectX::XMFLOAT2 tex;

			XMStoreFloat3(&pos, (normal - side1 - side2) * size);
			DirectX::XMStoreFloat3(&norm, normal);
			DirectX::XMStoreFloat2(&tex, textureCoordinates[0]);
			vertices.emplace_back(pos, tex, norm);

			XMStoreFloat3(&pos, (normal - side1 + side2) * size);
			XMStoreFloat2(&tex, textureCoordinates[1]);
			vertices.emplace_back(pos, tex, norm);

			XMStoreFloat3(&pos, (normal + side1 + side2) * size);
			XMStoreFloat2(&tex, textureCoordinates[2]);
			vertices.emplace_back(pos, tex, norm);

			XMStoreFloat3(&pos, (normal + side1 - side2) * size);
			XMStoreFloat2(&tex, textureCoordinates[3]);
			vertices.emplace_back(pos, tex, norm);

		}
		std::unique_ptr<Mesh<Geometry::LOD_VertexMin>> mesh(new Mesh<Geometry::LOD_VertexMin>(vertices, indices));
		return mesh;
	
}

MeshFactory::MeshFactory()
{
}


MeshFactory::~MeshFactory()
{
	
}
