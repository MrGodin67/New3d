#pragma once
#include "Geometry.h"
#include "MeshFactory.h"
#include "ObjParser.h"
#include <d3d11.h>
#include "Utilities.h"
#include "TextureFactory.h"
class Model2
{
	std::vector<std::unique_ptr<MeshFactory::Mesh<Geometry::LOD_VertexMin>>> m_meshes;
	ObjParser<VertexPTN> parser;
	Utilities::BoundingBox aabb;
	std::vector<ID3D11ShaderResourceView*> m_textures;
public:
	Model2() = default;
	~Model2()
	{
		for (size_t i = 0; i < m_textures.size(); i++)
			m_textures[i]->Release();
	}
	bool LoadObjectFile(std::string filename);
	size_t NumMeshes() {return m_meshes.size();	}
	MeshFactory::Mesh<Geometry::LOD_VertexMin>* GetMesh(size_t index)
	{
		return m_meshes[index].get();
	};
	ObjParser<VertexPTN>* getParser() { return &parser; }
};