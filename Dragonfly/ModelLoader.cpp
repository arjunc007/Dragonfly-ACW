#include "stdafx.h"
#include "ModelLoader.h"
#include "DDSTextureLoader.h"
#include <fstream>

using namespace DirectX;

ModelLoader::ModelLoader()
{
}

ModelLoader::~ModelLoader()
{
}

bool ModelLoader::LoadGeometry(const LPCSTR fileName, int& vertexCount, std::vector<SimpleVertex>* const model) const
{
	std::ifstream fin;
	char input;
	int i;

	fin.open(fileName);
	if (fin.fail())
		return false;

	//Read upto value of vertex count
	fin.get(input);
	while (input != ':')
		fin.get(input);

	//Read in vertex count
	fin >> vertexCount;

	//Create model using vertex count that was read in
	//model->resize(vertexCount);

	//Read up to the beginning of the data
	fin.get(input);
	while (input != ':')
		fin.get(input);

	fin.get(input);
	fin.get(input);

	
	//Read in vertex data
	SimpleVertex vertex;
	for (i = 0; i < vertexCount; i++)
	{
		fin >> vertex.Pos.x >> vertex.Pos.y >> vertex.Pos.z;
		fin >> vertex.Texture.x >> vertex.Texture.y;
		fin >> vertex.Normal.x >> vertex.Normal.y >> vertex.Normal.z;
		model->push_back(vertex);
	}

	//Close model file
	fin.close();

	return true;
}

bool ModelLoader::LoadTexture(ID3D11Device* device, const LPCWSTR textureFileName, ID3D11ShaderResourceView** texture) const
{
	HRESULT result;

	//Initialize texture object
	result = CreateDDSTextureFromFile(device, textureFileName, nullptr, texture);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}