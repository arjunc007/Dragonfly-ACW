#pragma once
#include <d3d11_1.h>
#include "Model.h"
#include "SimpleVertex.h"
#include <vector>

class ModelLoader
{
public:
	ModelLoader();
	ModelLoader(const ModelLoader&);
	~ModelLoader();

	bool LoadGeometry(const LPCSTR, int&, std::vector<SimpleVertex>* const) const;
	bool LoadTexture(ID3D11Device*, const LPCWSTR, ID3D11ShaderResourceView**) const;
};

