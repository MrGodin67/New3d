#include "PercompIncludes.h"
#include "Quad.h"
using namespace DirectX;
QuadPTN::QuadPTN()
{
	std::vector<Geometry::LOD_VertexMin> verts;
	std::vector<DWORD> inds;
	verts.resize(6);
	verts[0].pos = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	verts[0].tex = XMFLOAT2(0.0f, 0.0f);
	verts[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	verts[1].pos = XMFLOAT3(1.0f, 1.0f, 0.0f);
	verts[1].tex = XMFLOAT2(1.0f, 0.0f);
	verts[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	verts[2].pos = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	verts[2].tex = XMFLOAT2(0.0f, 1.0f);
	verts[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	verts[3].pos = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	verts[3].tex = XMFLOAT2(0.0f, 1.0f);
	verts[3].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	verts[4].pos = XMFLOAT3(1.0f, 1.0f, 0.0f);
	verts[4].tex = XMFLOAT2(1.0f, 0.0f);
	verts[4].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	verts[5].pos = XMFLOAT3(1.0f, -1.0f, 0.0f);
	verts[5].tex = XMFLOAT2(1.0f, 1.0f);
	verts[5].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	for (DWORD i = 0; i < 6; i++)
		inds.push_back(i);
	mesh = MeshFactory::CreateMesh<Geometry::LOD_VertexMin>(verts, inds);
}

MeshFactory::Mesh<Geometry::LOD_VertexMin>* QuadPTN::GetMesh()
{
	return mesh.get();
}
