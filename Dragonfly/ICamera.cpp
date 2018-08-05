#include "ICamera.h"
#include <DirectXMath.h>

using namespace DirectX;

ICamera::ICamera() : m_cameraBuffer(nullptr)
{
}

ICamera::ICamera(ID3D11Device* device, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& lookAt) : m_position(position),
m_lookAt(lookAt), m_up(XMFLOAT3(0.0f, 1.0f, 0.0f)), m_right(XMFLOAT3(1.0f, 0.0f, 0.0f)), m_cameraBuffer(nullptr)
{
	HRESULT result;
	D3D11_BUFFER_DESC cameraBufferDesc;

	// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBuffer);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cameraBufferDesc, nullptr, &m_cameraBuffer);
	if (FAILED(result))
	{
		return;
	}
}

void ICamera::SetPosition(const XMFLOAT3& pos)
{
	m_position = pos;
}

void ICamera::SetLookAt(const XMFLOAT3& lookAt)
{
	m_lookAt = lookAt;
}

void ICamera::SetUp(const XMFLOAT3& up)
{
	m_up = up;
}

void ICamera::SetRight(const XMFLOAT3& right)
{
	m_right = right;
}

ICamera::~ICamera()
{
}

void ICamera::Shutdown()
{
	// Release the camera constant buffer.
	if (m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = nullptr;
	}
}

void ICamera::Pitch(const float angle)
{
	// Rotate look vector about the right vector.

	const XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);

	XMVECTOR l = XMLoadFloat3(&m_lookAt);
	const XMVECTOR p = XMLoadFloat3(&m_position);
	l = XMVector3Transform(l, XMMatrixTranslationFromVector(-p));
	l = XMVector3TransformNormal(l, R);
	l = XMVector3Transform(l, XMMatrixTranslationFromVector(p));
	XMStoreFloat3(&m_lookAt, l);
}

void ICamera::RotateY(const float angle)
{
	// Rotate the basis vectors about the world y-axis.

	const XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), R));
	const XMVECTOR p = XMLoadFloat3(&m_position);
	XMVECTOR l = XMLoadFloat3(&m_lookAt);
	l = XMVector3Transform(l, XMMatrixTranslationFromVector(-p));
	l = XMVector3Transform(l, R);
	l = XMVector3Transform(l, XMMatrixTranslationFromVector(p));
	XMStoreFloat3(&m_lookAt, l);
}

void ICamera::Strafe(const float d)
{
	//m_position += d*m_right;
	const XMVECTOR s = XMVectorReplicate(d);
	const XMVECTOR p = XMLoadFloat3(&m_position);
	const XMVECTOR r = XMLoadFloat3(&m_right);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, r, p));
}

void ICamera::Walk(const float d)
{
	//m_position += d*m_lookAt;
	const XMVECTOR s = XMVectorReplicate(d);
	const XMVECTOR p = XMLoadFloat3(&m_position);
	const XMVECTOR l = XMLoadFloat3(&m_lookAt) - XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, l, p));
}

void ICamera::MoveVertical(const float d)
{
	//m_position += d*m_Up;
	const XMVECTOR s = XMVectorReplicate(d);
	const XMVECTOR p = XMLoadFloat3(&m_position);
	const XMVECTOR u = XMLoadFloat3(&m_up);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(s, u, p));
}

void ICamera::Render(ID3D11DeviceContext* deviceContext)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	CameraBuffer* dataPtr;

	// Lock the camera constant buffer so it can be written to.
	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = static_cast<CameraBuffer*>(mappedResource.pData);

	// Copy the camera position into the constant buffer.
	dataPtr->cameraPosition = m_position;
	dataPtr->padding = 0.0f;

	// Unlock the camera constant buffer.
	deviceContext->Unmap(m_cameraBuffer, 0);

	// Set the position of the camera constant buffer in the vertex shader.
	const int bufferNumber = 2;

	// Now set the camera constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);
	// Finally create the view matrix from the three updated vectors.
	const XMVECTOR eye = GetPositionXM(); 
	const XMVECTOR focus = GetLookAtXM();
	m_viewMatrix = XMMatrixLookAtLH(eye, focus, XMLoadFloat3(&m_up));

	return;
}

void ICamera::Update(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& lookAt)
{}

const XMMATRIX& ICamera::GetViewMatrix(XMMATRIX& output) const
{
	output = m_viewMatrix;
	return output;
}

const XMFLOAT3& ICamera::GetPosition() const
{
	return m_position;
}

const XMVECTOR& ICamera::GetPositionXM() const
{
	return XMLoadFloat3(&m_position);
}

const XMFLOAT3& ICamera::GetLookAt() const
{
	return m_lookAt;
}

const XMVECTOR& ICamera::GetLookAtXM() const
{
	return XMLoadFloat3(&m_lookAt);
}

const XMFLOAT3& ICamera::GetUp() const
{
	return m_up;
}

const XMVECTOR& ICamera::GetUpXM() const
{
	return XMLoadFloat3(&m_up);
}

const XMFLOAT3& ICamera::GetRight() const
{
	return m_right;
}

const XMVECTOR& ICamera::GetRightXM() const
{
	return XMLoadFloat3(&m_right);
}