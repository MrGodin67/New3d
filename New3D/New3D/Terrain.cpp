#include "PercompIncludes.h"
#include "Terrain.h"
#include <functional>
#include "Graphics.h"
#include "StringHandler.h"
#include "Locator.h"
#include "TextureFactory.h"
#include "Utilities.h"

using namespace DirectX;
Terrain::Terrain( TerrainDesc& desc)
	:m_width(desc.width),m_height(desc.height),
	m_position(desc.position), m_heightOffset(desc.heightOffset)
{
	
	m_heightMap.resize(m_width , m_height);
	if (! LoadRawHeightMap(desc.heightMapFilename))
	{
		throw Direct3D_EXCEPTION(E_FAIL, L"Unable to load terrain height map");
	};

	if (!LoadColorMap(desc.colorMapFilename))
	{
		throw Direct3D_EXCEPTION(E_FAIL, L"Unable to load terrain color map");
	};
	if (!CalculateNormals())
	{
		throw Direct3D_EXCEPTION(E_FAIL, L"Unable to calculate terrain normals");
	};
	if(!CreateModel())
	{
		throw Direct3D_EXCEPTION(E_FAIL, L"Unable to create terrain model");
	};
	if(!CalculateVectors())
	{
		throw Direct3D_EXCEPTION(E_FAIL, L"Unable to calculate terrain vectors");
	};
		;
	if(!LoadTerrainCells(Locator::GetDevice()))
	{
		throw Direct3D_EXCEPTION(E_FAIL, L"Unable to create terrain cell(s)");
	};;
	if (FAILED(CreateTextures(desc.textureFilenames)))
	{
		throw Direct3D_EXCEPTION(E_FAIL, L"Unable to create terrain textures");
	};;
}

Terrain::~Terrain()
{
	SafeDeleteArray(m_model);
	
	
	for (Table<TerrainCell*>::iter it = m_cellMap.begin(); it != m_cellMap.end(); it++)
	{
		(*it)->Shutdown();
		SafeDelete(*it);
	}

	for (auto& it : m_textures)
		SafeRelease(it.second);
	
}

void Terrain::FindCellsInView( CFirstPersonCamera::Frustum & frustrum)
{
	m_RenderCells.clear();
	for (size_t j = 0; j < ((m_height -1) / m_cellHeight); j++)
	{
		for (size_t i = 0; i < ((m_width - 1) / m_cellWidth); i++)
		{
			auto& cell = m_cellMap(j,i);
			if (frustrum.CheckBox(cell->BoundingBox().minPt, cell->BoundingBox().maxPt))
			{
				m_RenderCells.push_back(cell);
			}
		}
	}
	
}



size_t Terrain::RenderCount()
{
	return m_RenderCells.size();
}

std::vector<ID3D11ShaderResourceView*> Terrain::GetTextures()
{
	std::vector<ID3D11ShaderResourceView*> t;
	for (auto& it : m_textures)
		t.push_back(it.second);

	return t;
}

ID3D11ShaderResourceView * Terrain::GetTexture(std::string name)
{
	return m_textures[name];

}

TerrainCell * Terrain::RenderCells(size_t index)
{
	return m_RenderCells[index];
}

bool Terrain::GetHeight(DirectX::XMVECTOR pos, float& height)
{
	int index = 0;
	float Z = XMVectorGetZ(pos);
	int row = (int)((256.0f - Z) / m_cellHeight);
	int col = (int)(XMVectorGetX(pos) / m_cellWidth) ;
	auto& cell = m_cellMap(row, col);
				
	for (int i = 0; i < cell->GetVertexCount() / 3; i++)
	{
		index = i * 3;
		Utilities::Plane plane;
		plane.pt0 = XMFLOAT3(cell->m_vertexList[index].x,
			cell->m_vertexList[index].y,
			cell->m_vertexList[index].z);
		index++;
		plane.pt1 = XMFLOAT3(cell->m_vertexList[index].x,
			cell->m_vertexList[index].y,
			cell->m_vertexList[index].z);
		index++;
		plane.pt2 = XMFLOAT3(cell->m_vertexList[index].x,
			cell->m_vertexList[index].y,
			cell->m_vertexList[index].z);
		plane.MakeNormal();
		XMVECTOR dir = XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f);
		XMFLOAT3 linePt1;
		XMStoreFloat3(&linePt1, pos);
		XMFLOAT3 linePt2;
		XMFLOAT3 intersectPt;
		XMStoreFloat3(&linePt2, pos + dir *200.0f);
		Utilities::LineSegment line(linePt1, linePt2);
		float distance;
		if (Collision::LineIntersectPlane(line,plane,intersectPt,distance))
		{
			height = intersectPt.y;
			return true;
		}
	}
		
	
	return false;
}

TerrainIntersection Terrain::RayIntersectInView(Utilities::LineSegment& line)
{
	int index = 0;
	
	std::vector<TerrainCell*> tc = m_RenderCells;
	std::sort(tc.begin(), tc.end(),[&line](TerrainCell* A, TerrainCell* B)
	{
		DirectX::XMFLOAT3 AA = A->BoundingBox().center;
		DirectX::XMFLOAT3 BB = B->BoundingBox().center;
		DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&AA);
		DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&BB);
		DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&line.pt0);
		return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(a - c)) <  DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(b - c));

		
	});
	
	for (size_t ii = 0; ii < tc.size(); ii++)
	{
		auto& cell = tc[ii];
		index = 0;
		std::vector<TerrainIntersection> intersects;
		for(auto it : cell->m_tri)
		{
			TerrainIntersection inter;
			if (Collision::LineIntersectPlane(line, it, inter.intersectPoint, inter.distance))
			{
				inter.hit = true;
				intersects.push_back(inter);
			}
		}
		if (intersects.size() > 1)
		{
			std::sort(intersects.begin(), intersects.end(), [](TerrainIntersection& a, TerrainIntersection& b)
			{
				return a.distance < b.distance;
			});
			return intersects[0];
		}else if((intersects.size() == 1))
		   return intersects[0];
	}
	return TerrainIntersection();
}

HRESULT Terrain::CreateTextures( std::vector<std::string> filenames)
{
	HRESULT hr = S_OK;
	
	for (size_t i = 0; i < filenames.size(); i++)
	{
		
		if (DXStringHandler::DXDoesFileExist(filenames[i]))
		{
			std::string name;
			DXStringHandler::DXExtractNameFromFile(filenames[i], name);
		
			if (FAILED(hr = TextureFactory::CreateSRV(&m_textures[name], filenames[i])))
			{return E_FAIL;}// failed to create texture

		}else{return E_FAIL;};// file does not exist
	}
	return S_OK;
}

bool Terrain::LoadRawHeightMap(std::string m_terrainFilename)
{
	int error;
	FILE* filePtr;
	unsigned long long imageSize, count;
	USHORT* rawData;

	error = fopen_s(&filePtr, m_terrainFilename.c_str(), "rb");
	assert(error == 0);

	imageSize = m_height * m_width;
	rawData = new USHORT[imageSize];
	count = fread(rawData, sizeof(USHORT), imageSize, filePtr);
	assert(count == imageSize);
	error = fclose(filePtr);
	assert(error == 0);
	// Copy the image data into the height map table.
	for (int r = 0; r < m_height; r++)
	{
		for (int c = 0; c < m_width; c++)
		{
			int index = r * m_width + c;
			m_heightMap(r, c).pos = XMFLOAT3((float)c, (float)rawData[index], -(float)r);
			m_heightMap(r, c).pos.y /= m_heightOffset;
			m_heightMap(r, c).pos.z += (float)(m_height - 1);
		}
	}

	SafeDeleteArray( rawData );
	
	return true;
}

bool Terrain::CalculateNormals()
{
	
	XMFLOAT3 vertex1, vertex2, vertex3, vector1, vector2, sum, length;
	XMFLOAT3* normals;


	
	normals = new XMFLOAT3[(m_height - 1) * (m_width - 1)];
	
	for (int r = 0; r<(m_height - 1); r++)
	{
		for (int c = 0; c<(m_width - 1); c++)
		{
			/*vertex3 = float3(m_heightMap(r + 1, c).pos);
			vertex2 = float3(m_heightMap(r + 1, c + 1).pos);
			vertex1 = float3(m_heightMap(r, c + 1).pos);*/
			vertex1 = m_heightMap(r + 1 , c).pos;
			vertex2 = m_heightMap(r + 1 , c + 1).pos;
			vertex3 = m_heightMap(r, c ).pos;
			// Calculate the two vectors for this face.
			Utilities::Plane plane(vertex1, vertex2, vertex3);
			plane.MakeNormal();
			const int index = (r * (m_width - 1)) + c;
			normals[index] = plane.normal;
			
		}
	}
	int count = 0;
	// Now go through all the vertices and take a sum of the face normals that touch this vertex.
	for (int r = 0; r < m_height; r++)
	{
		for (int c = 0; c<m_width; c++)
		{
			// Initialize the sum.
			sum = XMFLOAT3(0.0f, 0.0f, 0.0f);

			// Bottom left face.
			if (((c - 1) >= 0) && ((r - 1) >= 0))
			{
				int index = ((r - 1) * (m_width - 1)) + (c - 1);
				sum.x += normals[index].x;
				sum.y += normals[index].y;
				sum.z += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((c<(m_width - 1)) && ((r - 1) >= 0))
			{
				int index = ((r - 1) * (m_width - 1)) + r;

				sum.x += normals[index].x;
				sum.y += normals[index].y;
				sum.z += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((c - 1) >= 0) && (r<(m_height - 1)))
			{
				int index = (r * (m_width - 1)) + (c - 1);
				sum.x += normals[index].x;
				sum.y += normals[index].y;
				sum.z += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((c < (m_width - 1)) && (r < (m_height - 1)))
			{
				int index = (r * (m_width - 1)) + c;
				sum.x += normals[index].x;
				sum.y += normals[index].y;
				sum.z += normals[index].z;
				count++;
			}
			// Average
			sum.x /= (float)count;
			sum.y /= (float)count;
			sum.z /= (float)count;
			XMStoreFloat3(&m_heightMap(r, c).normal,XMVector3Normalize(XMLoadFloat3(&sum)));
			
			
		}
	}
	
	
	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

bool Terrain::LoadColorMap(std::string m_colorMapFilename)
{
	int error, imageSize,  k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;


	// Open the color map file in binary.
	error = fopen_s(&filePtr, m_colorMapFilename.c_str(), "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Make sure the color map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biWidth != m_width) || (bitmapInfoHeader.biHeight != m_height))
	{
		return false;
	}

	// Calculate the size of the bitmap image data.
	// Since this is non-divide by 2 dimensions (eg. 257x257) need to add extra byte to each line.
	imageSize = m_height * ((m_width * 3));// +1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the color map portion of the height map structure.
	for (int r = 0; r < m_height; r++)
	{
		for (int c = 0; c<m_width; c++)
		{
			// Bitmaps are upside down so load bottom to top into the array.
			index = (m_width * (m_height - 1 - r)) + c;
			int R = index / m_width;
			int C = index % m_width;
			m_heightMap(R, C).color = XMFLOAT3((float)bitmapImage[k + 2] / 255.0f,
				(float)bitmapImage[k + 1] / 255.0f,
				(float)bitmapImage[k] / 255.0f);

			k += 3;
		}

		k++;
	}

	
	SafeDeleteArray(bitmapImage);
	


	return true;
}

bool Terrain::LoadMap(std::string filename)
{
	FILE* filePtr;
	unsigned long long imageSize, count;
	unsigned short* rawImage;
	UINT error;

	error = fopen_s(&filePtr, filename.c_str(), "rb");
	if (error != 0)
	{
		return false;
	}

	// Calculate the size of the raw image data.
	imageSize = m_height * m_width;

	// Allocate memory for the raw image data.
	rawImage = new unsigned short[imageSize];
	if (!rawImage)
	{
		return false;
	}

	// Read in the raw image data.
	count = fread(rawImage, sizeof(unsigned short), imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	
	for (int r = 0; r < m_height; r++)
	{
		for (int c = 0; c<m_width; c++)
		{
			const int index = r * m_width + c;
			m_heightMap(r,c).pos.y = (float)rawImage[index];
		}
	}

	
	SafeDeleteArray(rawImage);
	return true;
}

bool Terrain::CreateModel()
{
	m_vertexCount = (m_height - 1) * (m_width - 1) * 6;
	m_model = new ModelType[m_vertexCount];
	int index = 0;
	
	
	for (int r = 0; r<(m_height - 1); r++)
	{
		for (int c = 0; c<(m_width - 1); c++)
		{
			// Get the indexes to the four points of the quad.
			
			m_model[index].pos = m_heightMap(r, c).pos;
			m_model[index].tex = XMFLOAT2(0.0f, 0.0f);
			m_model[index].normal = m_heightMap(r, c).normal;
			m_model[index].color = m_heightMap(r, c).color;
			index++;
			
			m_model[index].pos = m_heightMap( r, c + 1 ).pos;
			m_model[index].tex = XMFLOAT2(1.0f, 0.0f);
			m_model[index].normal = m_heightMap(r, c + 1).normal;
			m_model[index].color = m_heightMap(r, c + 1).color;
			index++;
			
			m_model[index].pos = m_heightMap(r + 1, c).pos;
			m_model[index].tex = XMFLOAT2(0.0f, 1.0f);
			m_model[index].normal = m_heightMap(r + 1, c ).normal;
			m_model[index].color = m_heightMap(r + 1, c ).color;
			index++;
		
			m_model[index].pos = m_heightMap(r + 1, c).pos;
			m_model[index].tex = XMFLOAT2(0.0f, 1.0f);
			m_model[index].normal = m_heightMap(r + 1, c).normal;
			m_model[index].color = m_heightMap(r + 1, c).color;
			index++;
			
			m_model[index].pos = m_heightMap(r, c + 1).pos;
			m_model[index].tex = XMFLOAT2(1.0f, 0.0f);
			m_model[index].normal = m_heightMap(r, c + 1).normal;
			m_model[index].color = m_heightMap(r, c + 1).color;
			index++;
			
			m_model[index].pos = m_heightMap(r + 1, c + 1).pos;
			m_model[index].tex = XMFLOAT2(1.0f, 1.0f);
			m_model[index].normal = m_heightMap(r + 1, c + 1).normal;
			m_model[index].color = m_heightMap(r + 1, c + 1).color;
			index++;
			
		}
	}
	m_heightMap.resize(0, 0);
	return true;
}

bool Terrain::CalculateVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	XMFLOAT3 tangent, binormal;


	// Calculate the number of faces in the terrain model.
	faceCount = m_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i<faceCount; i++)
	{
		// Get the three vertices for this face from the terrain model.
		vertex1.pos = m_model[index].pos;
		vertex1.tex = m_model[index].tex;
		vertex1.normal = m_model[index].normal;
		index++;
		vertex2.pos = m_model[index].pos;
		vertex2.tex = m_model[index].tex;
		vertex2.normal = m_model[index].normal;
		index++;
		vertex3.pos = m_model[index].pos;
		vertex3.tex = m_model[index].tex;
		vertex3.normal = m_model[index].normal;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Store the tangent and binormal for this face back in the model structure.
		m_model[index - 1].tangent = XMFLOAT3(tangent.x, tangent.y, tangent.z);
		m_model[index - 1].binormal = XMFLOAT3(binormal.x, binormal.y, binormal.z);

		m_model[index - 2].tangent = XMFLOAT3(tangent.x, tangent.y, tangent.z);
		m_model[index - 2].binormal = XMFLOAT3(binormal.x, binormal.y, binormal.z);

		m_model[index - 3].tangent = XMFLOAT3(tangent.x, tangent.y, tangent.z);
		m_model[index - 3].binormal = XMFLOAT3(binormal.x, binormal.y, binormal.z);


		
	}

	return true;
}

void Terrain::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3, XMFLOAT3& tangent, XMFLOAT3& binormal)
{
	XMFLOAT3 vector1, vector2;
	XMFLOAT2 tuVector, tvVector;
	float den;
	


	// Calculate the two vectors for this face.
	
	vector1.x = vertex2.pos.x - vertex1.pos.x;
	vector1.y = vertex2.pos.y - vertex1.pos.y;
	vector1.z = vertex2.pos.z - vertex1.pos.z;

	
	vector2.x = vertex3.pos.x - vertex1.pos.x;
	vector2.y = vertex3.pos.y - vertex1.pos.y;
	vector2.z = vertex3.pos.z - vertex1.pos.z;

	// Calculate the tu and tv texture space vectors.
	tuVector.x = vertex2.tex.x - vertex1.tex.x;
	tvVector.x = vertex2.tex.y - vertex1.tex.y;

	tuVector.y = vertex3.tex.x - vertex1.tex.x;
	tvVector.y = vertex3.tex.y - vertex1.tex.y;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector.x * tvVector.y - tuVector.y * tvVector.x);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector.y * vector1.x - tvVector.x * vector2.x) * den;
	tangent.y = (tvVector.y * vector1.y - tvVector.x * vector2.y) * den;
	tangent.z = (tvVector.y * vector1.z - tvVector.x * vector2.z) * den;

	binormal.x = (tuVector.x * vector2.x - tuVector.y * vector1.x) * den;
	binormal.y = (tuVector.x * vector2.y - tuVector.y * vector1.y) * den;
	binormal.z = (tuVector.x * vector2.z - tuVector.y * vector1.z) * den;

	
	XMStoreFloat3(&tangent, XMVector3Normalize(XMLoadFloat3(&tangent)));
	XMStoreFloat3(&binormal, XMVector3Normalize(XMLoadFloat3(&binormal)));
	return;
}

bool Terrain::LoadTerrainCells(ID3D11Device * device)
{
	int cellHeight, cellWidth, cellRowCount, i, j;
	


	// Set the height and width of each terrain cell to a fixed 33x33 vertex array.
	cellHeight = 33;
	cellWidth = 33;
	m_cellWidth = 32;
	m_cellHeight = 32;
	// Calculate the number of cells needed to store the terrain data.
	cellRowCount = (m_width - 1) / (cellWidth - 1);
	m_cellCount = cellRowCount * cellRowCount;
	m_cellMap.resize(cellRowCount, cellRowCount);
	// Create the terrain cell array.

	// Loop through and initialize all the terrain cells.
	for (j = 0; j<cellRowCount; j++)
	{
		for (i = 0; i<cellRowCount; i++)
		{
			m_cellMap(j, i) = new TerrainCell(device, m_model, i, j, cellHeight, cellWidth, m_width);
		}
	}

	return true;

}





