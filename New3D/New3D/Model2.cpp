#include "PercompIncludes.h"
#include "Model2.h"
#include <DirectxMath.h>
#include "Locator.h"
bool Model2::LoadObjectFile(std::string filename)
{
	
	std::vector<shape<VertexPTN>> shapes;
	parser.loadObject(filename , shapes);
	assert(shapes.size() > 0);
	
	m_textures.resize(shapes.size());
	float minX = FLT_MAX;
	float maxX = -FLT_MAX;
	float minY = FLT_MAX;
	float maxY = -FLT_MAX;
	float minZ = FLT_MAX;
	float maxZ = -FLT_MAX;
	for (size_t i = 0; i < shapes.size(); i++)
	{
		std::vector<Geometry::LOD_VertexMin> verts;
		verts.resize(shapes[i].vertex.size());
		HRESULT hr;
		
		hr = TextureFactory::CreateSRV(&m_textures[i], shapes[i].textureName);
		assert(hr == S_OK);
		for (int j = 0; j < shapes[i].vertex.size(); j++)
		{
			verts[j].pos = DirectX::XMFLOAT3(shapes[i].vertex[j].position.x,
				shapes[i].vertex[j].position.y, shapes[i].vertex[j].position.z);
			verts[j].tex = DirectX::XMFLOAT2(shapes[i].vertex[j].texture.u,
				shapes[i].vertex[j].texture.v);
			verts[j].normal = DirectX::XMFLOAT3(shapes[i].vertex[j].normal.x,
				shapes[i].vertex[j].normal.y, shapes[i].vertex[j].normal.z);
			minX = __min(verts[j].pos.x, minX);
			maxX = __max(verts[j].pos.x, maxX);
			minY = __min(verts[j].pos.y, minY);
			maxY = __max(verts[j].pos.y, maxY);
			minZ = __min(verts[j].pos.z, minZ);
			maxZ = __max(verts[j].pos.z, maxZ);

		}
		std::vector<DWORD> ind = shapes[i].indices;
		m_meshes.push_back(MeshFactory::CreateMesh(verts, ind));
		
	}
	aabb.maxPt = DirectX::XMFLOAT3(maxX, maxY, maxZ);
	aabb.minPt = DirectX::XMFLOAT3(minX, minY, minZ);
	aabb.center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	for(int c =0; c <m_textures.size();c++)
		m_meshes[c]->SetTexture(m_textures[c]);

	return false;
}
