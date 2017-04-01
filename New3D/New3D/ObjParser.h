#pragma once
#include <vector>
#include "StringHandler.h"
#include <unordered_map>
#include "Geometry.h"
#include "Utilities.h"
#include <DirectXMath.h>
template <typename T>
struct v3
{
	T x, y, z;
};
template <typename T>
struct v2
{
	T u, v;
};

static const int byteSizePTN = sizeof(v3<float>) + sizeof(v2<float>) + sizeof(v3<float>);
struct VertexPTN
{
	v3<float> position;
	v2<float> texture;
	v3<float> normal;
};
struct VertexPT
{
	v3<float> position;
	v2<float> texture;
};

struct faceData
{
	std::vector<v3<int>> indices;
};



template<class T>
class shape
{
	// TODO setup to parse proper vertex format
	const bool IsV_PTN = std::is_same<T, VertexPTN>::value;
	const bool IsV_PT = std::is_same<T, VertexPT>::value;
	
public:
	shape() = default;
	std::vector<T> vertex;
	std::vector<DWORD> indices;
	std::vector<faceData> faces;
	std::string textureName;
};
template<typename T>
class ObjParser
{
	std::vector<v3<float>> all_verts;//all vertices
	std::vector<v2<float>> all_texture; // all u,v coords
	std::vector<v3<float>> all_normals; // all normals
	std::vector<faceData> all_faces; // all faces
	std::vector<Utilities::Plane> all_tri;
	std::vector<shape<T>> all_shapes;// all shapes (vertex), one shape per texture
	std::unordered_map<std::string, std::string> texture_filenames;
	Utilities::BoundingBox box;
	std::vector<std::string> lines;
	void ParseVerticies(std::string data);
	void ParseNormals(std::string data);
	void ParseTexture(std::string data);
	void ParseFace(std::string data);
	void ParseShapes(int index);
	void ParseTextureNames(std::string filename);
public:
	ObjParser<T>() = default;

	bool loadObject(std::string filename, std::vector<shape<T>>& shapes)
	{
		assert(DXStringHandler::DXDoesFileExist(filename));
		DXStringHandler::DXLoadTextFile(filename, lines);
		std::vector<std::string> tokens;
		std::string material_filename;
		std::string path;
		int counter = 0;
		int first_mtrl = 0;
		while (counter < lines.size())
		{
			std::string str;
			tokens.clear();
			int numfaces;
			DXStringHandler::Tokenize(&tokens, lines[counter], " ");
			if (tokens[0] == "mtllib")
			{
				DXStringHandler::DXExtractFilePath(filename, path);
				material_filename = path +  tokens[1];
			}
			else if (tokens[0] == "v")
			{
				ParseVerticies(lines[counter]);
			}
			else if (tokens[0] == "vt")
			{
				ParseTexture(lines[counter]);
			}
			else if (tokens[0] == "vn")
			{
				ParseNormals(lines[counter]);
			}
			else if (tokens[0] == "usemtl")
			{
				if (first_mtrl == 0)
					first_mtrl = counter;
			}
			else if (tokens[0] == "f")
			{
				ParseFace(lines[counter]);
			}
			counter++;
		}
		
		ParseShapes(first_mtrl);
		ParseTextureNames(material_filename);
		for (size_t c = 0; c < all_shapes.size(); c++)
		{
			all_shapes[c].textureName = path + texture_filenames[all_shapes[c].textureName];
		}
		shapes = std::move(all_shapes);
		return true;
	}
	std::vector<v3<float>> allVerticies() { return all_verts; };
	std::vector<v2<float>> allTextureUV() { return all_texture; };
	std::vector<v3<float>> allNormals() { return all_normals; };
	std::vector<Utilities::Plane> allTriangle() { return all_tri; }
	Utilities::BoundingBox AABB() { return box; }
	
};
template<typename T>
void ObjParser<T>::ParseVerticies(std::string data)
{
	std::vector<std::string> tokens;
	DXStringHandler::Tokenize(&tokens, data, " ");
	
	if (tokens.size() == 4)
	{
		v3<float> v;
		v.x = (float)atof(tokens[1].c_str());
		v.y = (float)atof(tokens[2].c_str());
		v.z = (float)atof(tokens[3].c_str());
		box.minPt.x = __min(v.x, box.minPt.x);
		box.minPt.y = __min(v.y, box.minPt.y);
		box.minPt.z = __min(v.z, box.minPt.z);

		box.maxPt.x = __max(v.x, box.maxPt.x);
		box.maxPt.y = __max(v.y, box.maxPt.y);
		box.maxPt.z = __max(v.z, box.maxPt.z);
		all_verts.push_back(v);
	}
	
	box.center = DirectX::XMFLOAT3((box.maxPt.x - box.minPt.x)*0.5f,
		(box.maxPt.y - box.minPt.y)*0.5f, (box.maxPt.z - box.minPt.z)*0.5f);
}
template<typename T>
void ObjParser<T>::ParseNormals(std::string data)
{
	std::vector<std::string> tokens;
	DXStringHandler::Tokenize(&tokens, data, " ");
	if (tokens.size() == 4)
	{
		v3<float> v;
		v.x = (float)atof(tokens[1].c_str());
		v.y = (float)atof(tokens[2].c_str());
		v.z = (float)atof(tokens[3].c_str());
		all_normals.push_back(v);
	}
}
template<typename T>
void ObjParser<T>::ParseTexture(std::string data)
{
	std::vector<std::string> tokens;
	DXStringHandler::Tokenize(&tokens, data, " ");
	if (tokens.size() == 3)
	{
		v2<float> t;
		t.u = (float)atof(tokens[1].c_str());
		t.v = (float)atof(tokens[2].c_str());
		all_texture.push_back(t);
	}
}
template<typename T>
inline void ObjParser<T>::ParseFace(std::string data)
{
	std::vector<std::string> tokens;
	DXStringHandler::Tokenize(&tokens, data, " ");
	faceData fData;;
	static int index = 0;
	for (int c = 1; c < 4; c++)
	{
		std::vector<std::string> sub_tokens;
		DXStringHandler::Tokenize(&sub_tokens, tokens[c], "/");
		v3<int> v;
		v.x = atoi(sub_tokens[0].c_str());
		v.y = atoi(sub_tokens[1].c_str());
		v.z = atoi(sub_tokens[2].c_str());

		fData.indices.push_back(v);

	}
	v3<float> p0 = all_verts[fData.indices[0].x-1];
	v3<float> p1 = all_verts[fData.indices[1].x-1];
	v3<float> p2 = all_verts[fData.indices[2].x-1];
	all_tri.emplace_back(DirectX::XMFLOAT3(p0.x, p0.y, p0.z), 
		DirectX::XMFLOAT3(p1.x, p1.y, p1.z),
		DirectX::XMFLOAT3(p2.x, p2.y, p2.z));
	all_tri.back().MakeNormal();
	all_tri.back().index = index;
	DirectX::XMVECTOR N = DirectX::XMLoadFloat3(&all_tri.back().normal);
	index++;
	if (DirectX::XMVector3Equal(N, DirectX::XMVectorZero()))
	{
		all_tri.pop_back();
		index > 0 ? index-- : index = 0;

	}
	
	all_faces.push_back(fData);
}
template<typename T>
inline void ObjParser<T>::ParseTextureNames(std::string filename)
{
	std::vector<std::string> mtlData;
	//assert();
	bool result = DXStringHandler::DXLoadTextFile(filename, mtlData);
	if (!result)
		PostQuitMessage(0x0);
	std::vector<std::string> tokens;
	int index = 0;
	while (index < mtlData.size())
	{
		tokens.clear();
		DXStringHandler::Tokenize(&tokens, mtlData[index], " ");
		if (tokens[0] == "newmtl")
		{
			std::string str = tokens[1];
			tokens.clear();
			index++;
			DXStringHandler::Tokenize(&tokens, mtlData[index], " ");
			texture_filenames[str] = tokens[1];
			index++;
		}
		else
		{
			index++;
		}

	};
}
template<class T>
void ObjParser<T>::ParseShapes(int index)
{
	int count = 0;
	for (int i = index; i < lines.size(); i++)
	{
		std::string str = lines[i];
		std::vector < std::string> tokens;
		DXStringHandler::Tokenize(&tokens, str, " ");
		if (tokens[0] == "usemtl")
		{
			all_shapes.push_back(shape<T>());
			all_shapes.back().textureName = tokens[1];
		}
		else
		{
			all_shapes.back().faces.push_back(all_faces[count]);
			count++;
		}


	}
	int ind = 0;
	
	for (int c = 0; c < all_shapes.size(); c++)
	{
		ind = 0;
		for (int i = 0; i < all_shapes[c].faces.size(); i++)
		{
			faceData f = all_shapes[c].faces[i];
			for (int j = 0; j < f.indices.size(); j++)
			{
				v3<int> data = f.indices[j];

				VertexPTN vert;
				vert.position = all_verts[data.x - 1];
				vert.texture = all_texture[data.y - 1];
				vert.normal = all_normals[data.z - 1];
				all_shapes[c].vertex.push_back(vert);
				all_shapes[c].indices.push_back(ind);
				ind++;
			}
		}
	}
};