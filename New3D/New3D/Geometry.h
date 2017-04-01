#pragma once
#include "Locator.h"
class Geometry
{
	
public:
	struct  LOD_VertexMax
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 tex;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 tangent;
		DirectX::XMFLOAT3 binormal;
		DirectX::XMFLOAT3 color;
	};
	struct alignas(16) LOD_VertexMid
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 tex;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 color;
	};
	struct alignas(16) LOD_VertexMin
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT2 tex;
		DirectX::XMFLOAT3 normal;
		LOD_VertexMin() {}
		LOD_VertexMin(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 tex, DirectX::XMFLOAT3 norm)
			:pos(pos), tex(tex), normal(normal)
		{}
	};
	struct ColorVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		ColorVertex() = default;
		ColorVertex(DirectX::XMFLOAT3 p, DirectX::XMFLOAT4 c)
			:position(p), color(c)
		{}
	};
	const static D3D11_BUFFER_DESC DynamicVertexWrite;
	const static D3D11_BUFFER_DESC DynamicIndexWrite;
	const static D3D11_BUFFER_DESC DefaultReadWrite;

	static HRESULT CreateIndexBuffer(const UINT sizeInBytes, ID3D11Buffer** buffer);
	static HRESULT CreateVertexBuffer(const UINT sizeInBytes, ID3D11Buffer** buffer);
	static HRESULT CreateVertexBufferData(const void* data, const UINT sizeInBytes, ID3D11Buffer** buffer);
	static HRESULT CreateIndexBufferData(const void* data, const UINT sizeInBytes, ID3D11Buffer** buffer);
};