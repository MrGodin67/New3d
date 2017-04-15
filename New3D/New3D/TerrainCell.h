#pragma once
#include "Utilities.h"
using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: TerrainCell
////////////////////////////////////////////////////////////////////////////////
class TerrainCell
{
private:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
		float r, g, b;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
		XMFLOAT3 color;
	};

	struct VectorType
	{
		float x, y, z;
	};

	struct ColorVertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};
	bool Initialize(ID3D11Device*, void*, int, int, int, int, int);
public:
	TerrainCell() {}
	TerrainCell(ID3D11Device* device, void* terrainModelPtr, int nodeIndexX, int nodeIndexY,
		int cellHeight, int cellWidth, int terrainWidth);
	TerrainCell(const TerrainCell&);
	~TerrainCell();

	
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	void RenderLineBuffers(ID3D11DeviceContext* device);

	int GetVertexCount();
	int GetIndexCount();
	int GetLineBuffersIndexCount();
	ID3D11Buffer* GetVertexBuffer() { return m_vertexBuffer; }
	ID3D11Buffer* GetIndexBuffer() { return m_indexBuffer; }
	
	Utilities::BoundingBox BoundingBox();

private:
	bool InitializeBuffers(ID3D11Device*, int, int, int, int, int, ModelType*);
	void ShutdownBuffers();
	
	void CalculateCellDimensions();
	bool BuildLineBuffers(ID3D11Device*);
	void ShutdownLineBuffers();

public:
	VectorType* m_vertexList;
	VertexType m_vertexType;
private:
	int m_vertexCount, m_indexCount, m_lineIndexCount;
	ID3D11Buffer *m_vertexBuffer = nullptr, *m_indexBuffer = nullptr,
		*m_lineVertexBuffer = nullptr, *m_lineIndexBuffer = nullptr;

	DirectX::XMFLOAT3 m_Center;
	Utilities::BoundingBox m_BoundingBox;
	float m_maxWidth, m_maxHeight, m_maxDepth, m_minWidth, m_minHeight, m_minDepth;
	float m_positionX, m_positionY, m_positionZ;
};