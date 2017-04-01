#include "PercompIncludes.h"
#include "Geometry.h"

const D3D11_BUFFER_DESC Geometry::DynamicVertexWrite =
{
	0,D3D11_USAGE_DYNAMIC,
	D3D11_BIND_VERTEX_BUFFER,
	D3D11_CPU_ACCESS_WRITE,
	0,0
};
const D3D11_BUFFER_DESC Geometry::DynamicIndexWrite =
{
	0,D3D11_USAGE_DYNAMIC,
	D3D11_BIND_INDEX_BUFFER,
	D3D11_CPU_ACCESS_WRITE,
	0,0
};
const D3D11_BUFFER_DESC Geometry::DefaultReadWrite =
{
	0, D3D11_USAGE_DEFAULT,
	D3D11_BIND_UNORDERED_ACCESS ,
	D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE,
	0,0
};

HRESULT Geometry::CreateIndexBuffer(const UINT sizeInBytes, ID3D11Buffer** buffer)
{
	HRESULT hr = 0L;
	D3D11_BUFFER_DESC desc = DynamicIndexWrite;
	desc.ByteWidth = sizeInBytes;
	hr = Locator::GetDevice()->CreateBuffer(&desc, NULL, buffer);
	assert(hr == S_OK);
	return hr;
}



HRESULT Geometry::CreateVertexBuffer(const UINT sizeInBytes, ID3D11Buffer** buffer)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC desc = DynamicVertexWrite;
	desc.ByteWidth = sizeInBytes;
	hr = Locator::GetDevice()->CreateBuffer(&desc, NULL, buffer);
	assert(hr == S_OK);
	return hr;
}

HRESULT Geometry::CreateVertexBufferData(const void* data, const UINT sizeInBytes, ID3D11Buffer** buffer)
{
	HRESULT hr = 0;
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = data;
	D3D11_BUFFER_DESC desc = DynamicVertexWrite;
	desc.ByteWidth = sizeInBytes;
	hr = Locator::GetDevice()->CreateBuffer(&desc, &vertexData, buffer);
	assert(hr == S_OK);
	return hr;

}
HRESULT Geometry::CreateIndexBufferData(const void* data, const UINT sizeInBytes, ID3D11Buffer** buffer)
{
	HRESULT hr = 0;
	D3D11_SUBRESOURCE_DATA indexData = { 0 };
	indexData.pSysMem = data;
	D3D11_BUFFER_DESC desc = DynamicIndexWrite;
	desc.ByteWidth = sizeInBytes;
	hr = Locator::GetDevice()->CreateBuffer(&desc, &indexData, buffer);
	assert(hr == S_OK);
	return hr;

}