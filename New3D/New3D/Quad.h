#pragma once
#include "MeshFactory.h"

class QuadPTN
{
	std::unique_ptr<MeshFactory::Mesh<Geometry::LOD_VertexMin>> mesh;
public:
	QuadPTN();
	MeshFactory::Mesh<Geometry::LOD_VertexMin>* GetMesh();
};