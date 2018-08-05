#include "stdafx.h"
#include "Light.h"
#include "Configuration.h"

using namespace DirectX;

int Light::lightIndex = 0;

Light::Light(ID3D11Device* device) : m_lightBuffer(nullptr), m_specularPower(0.0f), lightNum(lightIndex++), m_focus(XMFLOAT3(0.0f, 0.0f, 0.0f))
{
	HRESULT result;
	D3D11_BUFFER_DESC lightPositionBufferDesc;
	// Setup the description of the dynamic constant buffer that is in the vertex shader.
	lightPositionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightPositionBufferDesc.ByteWidth = sizeof(LightPositionBuffer);
	lightPositionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightPositionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightPositionBufferDesc.MiscFlags = 0;
	lightPositionBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightPositionBufferDesc, nullptr, &m_lightPositionBuffer);
	if (FAILED(result))
	{
		return;
	}

	D3D11_BUFFER_DESC lightBufferDesc;
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer);
	if (FAILED(result))
	{
		return;
	}

}


Light::~Light()
{
}

void Light::Shutdown()
{
	//Release light constant buffer
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = nullptr;
	}

	if (m_lightPositionBuffer)
	{
		m_lightPositionBuffer->Release();
		m_lightPositionBuffer = nullptr;
	}
}

void Light::SetAmbientColor(const float& r, const float& g, const float& b, const float& a)
{
	m_ambientColor = XMFLOAT4(r, g, b, a);
	return;
}

void Light::SetDiffuseColor(const float& r, const float& g, const float& b, const float& a)
{
	m_diffuseColor = XMFLOAT4(r, g, b, a);
	return;
}

void Light::SetPosition(const float& x, const float& y, const float&z)
{
	m_position = XMFLOAT3(x, y, z);
	return;
}

void Light::SetSpecularColor(const float& r, const float& g, const float& b, const float& a)
{
	m_specularColor = XMFLOAT4(r, g, b, a);
	return;
}

void Light::SetSpecularPower(const float& power)
{
	m_specularPower = power;
	return;
}

const XMFLOAT4& Light::GetAmbientColor() const
{
	return m_ambientColor;
}

const XMFLOAT4& Light::GetDiffuseColor() const
{
	return m_diffuseColor;
}

const XMFLOAT3& Light::GetPosition() const
{
	return m_position;
}

const XMFLOAT4& Light::GetSpecularColor() const
{
	return m_specularColor;
}


const float& Light::GetSpecularPower() const
{
	return m_specularPower;
}

void Light::Render(ID3D11DeviceContext* deviceContext)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	LightPositionBuffer* dataPtr;
	LightBuffer* dataPtr2;

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(m_lightPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = static_cast<LightPositionBuffer*>(mappedResource.pData);

	// Copy the lighting variables into the constant buffer.
	dataPtr->lightPosition[lightNum] = m_position;
	dataPtr->lightFocus[lightNum] = m_focus;
	
	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightPositionBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	int bufferNumber = 1;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = static_cast<LightBuffer*>(mappedResource.pData);

	// Copy the lighting variables into the constant buffer.
	dataPtr2->ambientColor = m_ambientColor;
	dataPtr2->diffuseColor = m_diffuseColor;
	dataPtr2->specularColor = m_specularColor;
	dataPtr2->specularPower = m_specularPower;
	dataPtr2->attenuation = lightAttenuation;
	dataPtr2->range = lightRange;
	dataPtr2->cone = lightSpread;
	dataPtr2->padding = XMFLOAT2(0.0f, 0.0f);

	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);
}

void Light::SetFocus(const float x, const float y, const float z)
{
	m_focus.x = x;
	m_focus.y = y;
	m_focus.z = z;
}