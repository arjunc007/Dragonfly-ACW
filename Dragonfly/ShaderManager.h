//Code adapted from http://www.rastertek.com/tutdx11.html

#pragma once
#include <memory>
#include <DirectXMath.h>
#include "LightShader.h"
#include "ParticleShader.h"

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	bool Initialize(ID3D11Device*, const HWND);
	void Shutdown();
	
	//bool RenderTextureShader(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*);

	bool RenderLightShader(ID3D11DeviceContext*, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&) const;

	bool RenderParticleShader(ID3D11DeviceContext*, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&) const;

	//bool RenderBumpMapShader(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*,
	//		ID3D11ShaderResourceView*, DirectX::XMFLOAT3, DirectX::XMFLOAT4);

private:
	//TextureShaderClass* m_TextureShader;
	std::unique_ptr<LightShader> m_LightShader;
	std::unique_ptr<ParticleShader> m_ParticleShader;
	//BumpMapShaderClass* m_BumpMapShader;
};


