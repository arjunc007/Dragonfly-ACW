#include "EyeCam.h"
#include <DirectXMath.h>

using namespace DirectX;

EyeCam::EyeCam(ID3D11Device* device, const XMFLOAT3& position, const XMFLOAT3& lookAt) :
	ICamera(device, position, lookAt)
{
}


EyeCam::~EyeCam()
{
}

void EyeCam::MoveUp(const float d)
{}

void EyeCam::MoveSide(const float d)
{}

void EyeCam::MoveForward(const float d)
{}

void EyeCam::Update(const XMFLOAT3& lookAt, const XMFLOAT3& pos)
{
	SetPosition(pos);
	SetLookAt(lookAt);
}

void EyeCam::Reset()
{
	/*XMVECTOR dir = XMLoadFloat3(&XMFLOAT3(body.x - head.x, body.y - head.y, body.z - head.z));
	m_lookAt = XMLoadFloat3(&head);
	m_position = XMLoadFloat3(&head) - 0.5f*dir;
	m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);*/
}