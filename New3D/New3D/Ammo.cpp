#include "PercompIncludes.h"
#include "Ammo.h"
#include "Utilities.h"
using namespace DirectX;
ID3D11ShaderResourceView * AmmoQuad::texture = nullptr;
void AmmoQuad::Create(XMFLOAT3 scale, DirectX::XMVECTOR target, DirectX::XMVECTOR pos)
{
	mat_scale = XMMatrixScaling(scale.x, scale.y, scale.z);
	velocity = XMVector3Normalize(target - pos);
	position = pos;
	std::vector<Geometry::LOD_VertexMin> verts;
	std::vector<DWORD> inds;
	verts.resize(6);
	verts[0].pos = XMFLOAT3(-1.0f, 1.0f, 0.0f);
	verts[0].tex = XMFLOAT2(0.0f, 0.0f);
	verts[0].normal = XMFLOAT3(0.0f,  0.0f, - 1.0f);

	verts[1].pos = XMFLOAT3(1.0f,  1.0f ,0.0f);
	verts[1].tex = XMFLOAT2(1.0f ,0.0f);
	verts[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	verts[2].pos = XMFLOAT3(-1.0f, - 1.0f, 0.0f);
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
	speed = 0.01f;
}

DirectX::XMMATRIX AmmoQuad::GetWorldMatrix()
{
	return mat_scale * mat_rot * XMMatrixTranslation(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));
}

MeshFactory::Mesh<Geometry::LOD_VertexMin>* AmmoQuad::GetMesh()
{
	return mesh.get();
}

void AmmoQuad::Update(float dt, XMFLOAT3 cameraPosition)
{
	XMFLOAT3 modelPosition;
	XMStoreFloat3(&modelPosition, position);
	float angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0 / XM_PI);

	// Convert rotation into radians.
	float rotation = (float)angle * 0.0174532925f;

	// Setup the rotation the billboard at the origin using the world matrix.
	mat_rot =XMMatrixRotationY( rotation);
	position += velocity * speed * dt;
}

void AmmoQuad::SetTexture(ID3D11ShaderResourceView * tex)
{
	texture = tex;
}
