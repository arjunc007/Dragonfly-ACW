#include "stdafx.h"
#include "Camera.h"
#include <DirectXMath.h>

using namespace DirectX;


Camera::Camera() : m_positionX(0.0f), m_positionY(0.0f), m_positionZ(0.0f), m_rotationX(0.0f), m_rotationY(0.0f), m_rotationZ(0.0f)
{
}

Camera::Camera(const Camera& c)
{
	XMFLOAT3 pos = c.GetPosition();
	m_positionX = pos.x;
	m_positionY = pos.y;
	m_positionZ = pos.z;

	XMFLOAT3 rot = c.GetRotation();
	m_rotationX = rot.x;
	m_rotationY = rot.y;
	m_rotationZ = rot.z;
}


Camera::~Camera()
{
}

void Camera::SetPosition(const float& x, const float& y, const float& z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void Camera::SetRotation(const float& x, const float& y, const float& z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}

XMFLOAT3 Camera::GetPosition() const
{
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 Camera::GetRotation() const
{
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void Camera::Render()
{
	XMVECTOR up, position, lookAt;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Setup the position of the camera in the world.
	position = XMVectorSet(m_positionX, m_positionY, m_positionZ, 0.0f);

	// Setup where the camera is looking by default.
	lookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3TransformCoord(lookAt, rotationMatrix);
	up = XMVector3TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);

	return;
}

void Camera::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}