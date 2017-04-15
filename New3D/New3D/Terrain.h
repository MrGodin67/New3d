#pragma once

#include "Table.h"

#include "TerrainCell.h"

#include "Camera3D_2.h"
#include "Collision.h"
#include <memory>


struct HeightMapType
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT3 color;
};

struct ModelType
{
	XMFLOAT3 pos;
	XMFLOAT2 tex;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 binormal;
	XMFLOAT3 color;
	
};

struct VectorType
{
	float x, y, z;
};

struct TempVertexType
{
	XMFLOAT3 pos;
	XMFLOAT2 tex;
	XMFLOAT3 normal;
};
class Terrain 
{
public:
	struct TerrainDesc
	{
		std::string heightMapFilename;
		std::string colorMapFilename;
		std::vector<std::string> textureFilenames;
		int width  = 0;
		int height = 0;
		float heightOffset = 0.0f;
		XMFLOAT3 position = { 0.0f,0.0f,0.0f };
		TerrainDesc() {}
		TerrainDesc(XMFLOAT3 pos,std::string hmFilename, std::string cmFilename, int w, int h,
			float hOffset,std::vector<std::string> texFiles)
			:position(pos),heightMapFilename(hmFilename), colorMapFilename(cmFilename),
			width(w), height(h),heightOffset(hOffset),textureFilenames(texFiles)
		{}

	};
	
private:
	int m_width = 0;
	int m_height = 0;
	int m_vertexCount;
	DirectX::XMFLOAT3 m_position;
	float m_heightOffset = 64.0f; 
	int m_cellCount;
	int m_cellWidth;
	int m_cellHeight;
	Table<HeightMapType> m_heightMap;
	Table<TerrainCell*> m_cellMap;
	ModelType* m_model = nullptr;
	
	std::vector<TerrainCell*> m_RenderCells;
	std::unordered_map<std::string,ID3D11ShaderResourceView*> m_textures;
	XMMATRIX m_matWorld = XMMatrixIdentity();
private:

	bool LoadRawHeightMap(std::string m_terrainFilename);
	bool CalculateNormals();
	bool LoadColorMap(std::string m_colorMapFilename);
	bool LoadMap(std::string filename);
	bool CreateModel();
	bool CalculateVectors();
	void CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, DirectX::XMFLOAT3& tangent, DirectX::XMFLOAT3& binormal);
	bool LoadTerrainCells(ID3D11Device* device);
	HRESULT CreateTextures( std::vector<std::string> filenames);

public:
	Terrain() {}
	Terrain(TerrainDesc& desc);
	~Terrain();
	void  FindCellsInView( CFirstPersonCamera::Frustum& frustrum);
	size_t RenderCount();
	std::vector<ID3D11ShaderResourceView*> GetTextures();
	ID3D11ShaderResourceView* GetTexture(std::string name);
	TerrainCell* RenderCells(size_t index);
	bool GetHeight(DirectX::XMVECTOR pos, float& height);
};