#include "stdafx.h"
#include "Model.h"
#include <DirectXMath.h>
#include "Configuration.h"

using namespace DirectX;

Model::Model() : m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_modelBuffer(nullptr), m_vertexCount(0), m_indexCount(0), m_texture(nullptr), m_vertices(0)
{
}

Model::~Model()
{
}

bool Model::Initialize(ID3D11Device* device, const LPCSTR modelFile, const LPCWSTR textureFile)
{
	bool result;

	const std::unique_ptr<ModelLoader> loader = std::make_unique<ModelLoader>();

	//SimpleVertex* tmp = m_vertices.get();
	result = loader->LoadGeometry(modelFile, m_vertexCount, &m_vertices);
	//m_vertices.reset(tmp);
	m_indexCount = m_vertexCount;

	result = loader->LoadTexture(device, textureFile, &m_texture);

	//Initialize vertex and index buffers that hold geometry
	result = InitializeBuffers(device);
	if (!result)
		return false;

	return true;
}

void Model::Shutdown()
{
	//Release vertex and index buffers
	ShutdownBuffers();

	//Release model data
	ReleaseModel();

	return;
}

void Model::Render(ID3D11DeviceContext* deviceContext)
{
	//Put vertex and index buffers on graphics pipeline to prepare for drawing
	RenderBuffers(deviceContext);

	Draw(deviceContext);
	return;
}

int Model::GetVertexCount() const
{
	return m_vertexCount;
}

int Model::GetIndexCount() const
{
	return m_indexCount;
}

ID3D11ShaderResourceView* Model::GetTexture() const
{
	return m_texture;
}

bool Model::InitializeBuffers(ID3D11Device* device)
{
	SimpleVertex* vertices;
	WORD* indices;
	HRESULT result;
	int i;
	
	// Create the vertex array.
	vertices = new SimpleVertex[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	indices = new WORD[m_indexCount];
	if (!indices)
	{
		return false;
	}
	
	// Load the vertex array with data.

	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i] = m_vertices[i];

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(WORD) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	D3D11_BUFFER_DESC modelBufferDesc;
	modelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	modelBufferDesc.ByteWidth = sizeof(ModelBuffer);
	modelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	modelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	modelBufferDesc.MiscFlags = 0;
	modelBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&modelBufferDesc, nullptr, &m_modelBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Model::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	if (m_modelBuffer)
	{
		m_modelBuffer->Release();
		m_modelBuffer = nullptr;
	}

	if (m_texture)
	{
		m_texture->Release();
		m_texture = nullptr;
	}

	return;
}

bool Model::RenderBuffers(ID3D11DeviceContext* deviceContext) const
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer strides
	stride = sizeof(SimpleVertex);
	
	//Set buffer offsets
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set the constant buffer for model
	ModelBuffer* dataPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	HRESULT result;

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_modelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	const XMFLOAT3 scale = GetScale();
	const XMFLOAT4 rotation = GetRotation();
	const XMFLOAT3 pos = GetPosition();

	XMMATRIX worldMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	worldMatrix *= XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
	worldMatrix *= XMMatrixTranslation(pos.x, pos.y, pos.z);

	// Get a pointer to the data in the constant buffer.
	dataPtr = static_cast<ModelBuffer*>(mappedResource.pData);

	// Copy the matrices into the constant buffer.
	dataPtr->worldMatrix = XMMatrixTranspose(worldMatrix);

	// Unlock the constant buffer.
	deviceContext->Unmap(m_modelBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 3;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_modelBuffer);

	return true;
}

void Model::Scale(const float x, const float y, const float z)
{
	XMFLOAT3 scale = GetScale();
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
	SetScale(scale);
}

void Model::Rotate(const float x, const float y, const float z)
{
	XMFLOAT4 rotation = GetRotation();
	//x = pitch, y = yaw, z = roll
	const XMVECTOR delta = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z));
	XMVECTOR R = XMLoadFloat4(&rotation);
	R = XMQuaternionMultiply(delta, R);
	XMStoreFloat4(&rotation, R);
	SetRotation(rotation);
}

void Model::Move(const float x, const float y, const float z)
{
	XMFLOAT3 pos = GetPosition();
	pos.x += x;
	pos.y += y;
	pos.z += z;
	SetPosition(pos);
}

void Model::ReleaseModel()
{
	if (m_vertices.size()>0)
	{
		m_vertices.clear();
	}

	return;
}

void Model::Draw(ID3D11DeviceContext* deviceContext) const
{
	//Set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &m_texture);

	// Render the moodel.
	deviceContext->DrawIndexed(m_indexCount, 0, 0);
}

void Model::Reset()
{

}

void Model::CreateStick(ID3D11Device* device, const float x, const float y, const float z)
{
	Initialize(device, cubeFile, stickTexture);
	Scale(stickScale.x, stickScale.y, stickScale.z);
	Move(x, y, z);
}

void Model::CreateGround(ID3D11Device* device)
{
	Initialize(device, cubeFile, floorTexture);
	Scale(floorScale.x, floorScale.y, floorScale.z);
}

void Model::CreateHemisphere(ID3D11Device* device, const float size)
{
	Initialize(device, cubeFile, hemisphereTexture);
	Scale(size, size, size);
}