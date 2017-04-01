#pragma once

#include "Locator.h"
#include "Geometry.h"
#include <memory>
#include <string>
#include <vector>
static const char* MeshDefaultTexNames[][255] =
{
	{"texture01\0"},
	{"normal01\0"},
	{"bump01\0"}
};


class MeshFactory
{
public:
	template<typename VERT_STRUCT>
	class Mesh
	{
	private:
		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indexBuffer = nullptr;
		std::vector<ID3D11ShaderResourceView*> m_textures;
		UINT vertCount;
		UINT indexCount;
	public:
		Mesh() {}
		virtual ~Mesh()
		{
			Release();
		};
		Mesh(const std::vector<VERT_STRUCT> verts, const std::vector<DWORD>indices)
		{
			
			HRESULT hr = S_OK;
			UINT sizeInBytes = sizeof(DWORD) * (UINT)indices.size();
			hr = Geometry::CreateIndexBufferData(indices.data(), sizeInBytes, &m_indexBuffer);
			assert(hr == S_OK);
			sizeInBytes = sizeof(VERT_STRUCT) * (UINT)verts.size();
			hr = Geometry::CreateVertexBufferData(verts.data(), sizeInBytes, &m_vertexBuffer);
			assert(hr == S_OK);
			vertCount = (UINT)verts.size();
			indexCount = (UINT)indices.size();

		};
		VERT_STRUCT VertType;
		void Release(){
			m_vertexBuffer->Release();
			m_indexBuffer->Release();
			
		}
		ID3D11Buffer* GetVertexBuffer() { return m_vertexBuffer; }
		ID3D11Buffer* GetIndexBuffer() { return m_indexBuffer; }
		UINT VerticesCount() { return vertCount; }
		UINT IndicesCount() { return indexCount; }
		UINT VertexBytes() { return sizeof(VERT_STRUCT); }
		UINT VertexBufferSize() { return sizeof(VERT_STRUCT)*vertCount; }
		void SetTexture(ID3D11ShaderResourceView* texture);
		ID3D11ShaderResourceView* GetTexture(const size_t& index);
	};
	template<typename VERT_STRUCT>
	static std::unique_ptr<Mesh<VERT_STRUCT>> CreateMesh(const std::vector<VERT_STRUCT> verts, const std::vector<DWORD> indices)
	{
		std::unique_ptr < Mesh<VERT_STRUCT>> mesh(new Mesh<VERT_STRUCT>(verts, indices));
		return mesh;
	}
	static std::unique_ptr<Mesh<Geometry::LOD_VertexMin>> MakeCubePTN(float size);
public:
	MeshFactory();
	virtual ~MeshFactory();
};

template<typename VERT_STRUCT>
inline void MeshFactory::Mesh<VERT_STRUCT>::SetTexture(ID3D11ShaderResourceView * texture)
{
	m_textures.push_back(texture);
}

template<typename VERT_STRUCT>
inline ID3D11ShaderResourceView * MeshFactory::Mesh<VERT_STRUCT>::GetTexture(const size_t& index)
{
	return m_textures[index];
}
