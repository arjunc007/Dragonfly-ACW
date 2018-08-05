//Code adapted from http://www.rastertek.com/tutdx11.html

#include "stdafx.h"
#include "LightShader.h"
#include "Configuration.h"
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <fstream>

using namespace DirectX;

LightShader::LightShader() : m_vertexShader(nullptr), m_pixelShader(nullptr), m_layout(nullptr), m_sampleState(nullptr), m_constantBuffer(nullptr)
{
}

LightShader::~LightShader()
{
}

bool LightShader::Initialize(ID3D11Device* device, const HWND hwnd)
{
	//Initialize the vertex and pixel shaders
	if (!InitializeShader(device, hwnd, lightShaderVS, lightShaderPS))
		return false;
	return true;
}

void LightShader::Shutdown()
{
	//Shutdown vertex and pixel shaders
	ShutdownShader();
	return;
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext, const XMMATRIX& view, const XMMATRIX& projection) const
{
	//Set shader parameters used for rendering
	if (!SetShaderParameters(deviceContext, view, projection))
		return false;
	//Now render prepared buffer with shader
	RenderShader(deviceContext);

	return true;
}

bool LightShader::InitializeShader(ID3D11Device* device, const HWND hwnd, const LPCSTR vsName, const LPCSTR psName)
{
	HRESULT result;
	ID3DBlob* errorMessage = nullptr;
	ID3DBlob* vertexShaderBuffer = nullptr;
	ID3DBlob* pixelShaderBuffer = nullptr;
	unsigned int numElements;
	
	

	//Compile vertex shader code
	result = D3DCompileFromFile(fxFileName, nullptr, nullptr, vsName, "vs_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));
			errorMessage->Release();
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, fxFileName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	//Compile pixel shader code
	result = D3DCompileFromFile(fxFileName, nullptr, nullptr, psName, "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));
			errorMessage->Release();
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, fxFileName, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	if (errorMessage) errorMessage->Release();

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	
	// Get a count of the elements in the layout.
	numElements = ARRAYSIZE(layout);

	// Create the vertex input layout.
	result = device->CreateInputLayout(layout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;


	//Create texture sampler state description
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//Create texture sampler state
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
		return false;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cBufferDesc, nullptr, &m_constantBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void LightShader::ShutdownShader()
{
	//Release the sampler state
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = nullptr;
	}

	// Release the matrix constant buffer.
	if (m_constantBuffer)
	{
		m_constantBuffer->Release();
		m_constantBuffer = nullptr;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}

	return;
}

void LightShader::OutputShaderErrorMessage(ID3DBlob* errorMessage, const HWND hwnd, const LPCWSTR shaderFilename) const
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = static_cast<char*>(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = nullptr;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix) const
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBuffer* dataPtr;
	unsigned int bufferNumber;

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = static_cast<ConstantBuffer*>(mappedResource.pData);

	// Copy the matrices into the constant buffer.
	dataPtr->viewMatrix = XMMatrixTranspose(viewMatrix);
	dataPtr->projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Unlock the constant buffer.
	deviceContext->Unmap(m_constantBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_constantBuffer);

	return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext) const
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	//Set sampler state in the pixel shader
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	return;
}