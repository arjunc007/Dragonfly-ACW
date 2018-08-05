//Code adapted from http://www.rastertek.com/tutdx11.html

#include "ShaderManager.h"

using namespace DirectX;

ShaderManager::ShaderManager() : m_LightShader(nullptr)
{
}


ShaderManager::~ShaderManager()
{
}

bool ShaderManager::Initialize(ID3D11Device* device, const HWND hwnd)
{
	bool result;

	// Create the light shader object.
	m_LightShader = std::make_unique<LightShader>();
	if (!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the particle shader object.
	m_ParticleShader = std::make_unique<ParticleShader>();
	if (!m_ParticleShader)
	{
		return false;
	}

	// Initialize the particle shader object.
	result = m_ParticleShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}
	
	return true;
}

void ShaderManager::Shutdown()
{
	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		m_LightShader = nullptr;
	}

	// Release the particle shader object.
	if (m_ParticleShader)
	{
		m_ParticleShader->Shutdown();
		m_ParticleShader = nullptr;
	}

	return;
}
//
//bool ShaderManager::RenderTextureShader(ID3D11DeviceContext* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
//	ID3D11ShaderResourceView* texture)
//{
//	bool result;
//
//
//	// Render the model using the texture shader.
//	result = m_TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
//	if (!result)
//	{
//		return false;
//	}
//
//	return true;
//}


bool ShaderManager::RenderLightShader(ID3D11DeviceContext* deviceContext,const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix) const
{
	bool result;

	// Render the model using the light shader.
	result = m_LightShader->Render(deviceContext, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool ShaderManager::RenderParticleShader(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix) const
{
	bool result;

	// Render the model using the light shader.
	result = m_ParticleShader->Render(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

//bool ShaderManagerClass::RenderBumpMapShader(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
//	ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture, D3DXVECTOR3 lightDirection,
//	D3DXVECTOR4 diffuse)
//{
//	bool result;
//
//
//	// Render the model using the bump map shader.
//	result = m_BumpMapShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture, normalTexture, lightDirection, diffuse);
//	if (!result)
//	{
//		return false;
//	}
//
//	return true;
//}
