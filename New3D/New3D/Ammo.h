#pragma once

#include "MeshFactory.h"

class AmmoQuad
{
private:
	std::unique_ptr<MeshFactory::Mesh<Geometry::LOD_VertexMin>> mesh;
	DirectX::XMMATRIX mat_scale;
	DirectX::XMMATRIX mat_rot;
	DirectX::XMVECTOR position;
	DirectX::XMVECTOR velocity;
	float speed =0.01f;
	static ID3D11ShaderResourceView* texture;
public:
	AmmoQuad() = default;
	void Create(DirectX::XMFLOAT3 scale, DirectX::XMVECTOR target, DirectX::XMVECTOR pos);
	DirectX::XMMATRIX GetWorldMatrix();
	MeshFactory::Mesh<Geometry::LOD_VertexMin>* GetMesh();
	void Update(float dt, DirectX::XMFLOAT3 cameraPosition);
	static void SetTexture(ID3D11ShaderResourceView* tex);
};