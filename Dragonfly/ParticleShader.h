//Code adapted from http://www.rastertek.com/tutdx11.html

#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>

class ParticleShader
{
private:
	struct ConstantBuffer
	{
		DirectX::XMMATRIX worldMatrix;
		DirectX::XMMATRIX viewMatrix;
		DirectX::XMMATRIX projectionMatrix;
	};

public:
	ParticleShader();
	ParticleShader(ParticleShader&);
	ParticleShader& operator=(const ParticleShader&);
	~ParticleShader();

	bool Initialize(ID3D11Device*, const HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&) const;

private:
	bool InitializeShader(ID3D11Device*, const HWND, const LPCSTR, const LPCSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3DBlob*, const HWND, const LPCWSTR) const;

	bool SetShaderParameters(ID3D11DeviceContext*, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&) const;
	void RenderShader(ID3D11DeviceContext*) const;

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_constantBuffer;
};

