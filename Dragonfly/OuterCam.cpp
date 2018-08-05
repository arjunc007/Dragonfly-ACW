#include "OuterCam.h"
#include "Configuration.h"

using namespace DirectX;

const float step = 5.f*XM_PI / 180;

OuterCam::OuterCam(ID3D11Device* device, const XMFLOAT3& position, const XMFLOAT3& lookAt) :
	ICamera(device, position, lookAt), m_radius(hemisphere_radius_outer)
{
}


OuterCam::~OuterCam()
{
}

void OuterCam::MoveUp(const float f)
{
	const float d = f * step;
	if ((GetPosition().y < m_radius*0.9f || d < 0.f) && (GetPosition().y > 1.5f || d > 0.0f))
	{
		const XMMATRIX R = XMMatrixRotationAxis(GetRightXM(), d);
		const XMVECTOR p = XMVector3TransformNormal(GetPositionXM(), R);
		SetPosition(XMFLOAT3(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p)));
	}
}

void OuterCam::MoveSide(const float f)
{
	const float d = f * -step;
	const XMMATRIX R = XMMatrixRotationAxis(GetUpXM(), d);
	const XMVECTOR p = XMVector3TransformNormal(GetPositionXM(), R);
	const XMVECTOR r = XMVector3TransformNormal(GetRightXM(), R);
	SetPosition(XMFLOAT3(XMVectorGetX(p), XMVectorGetY(p), XMVectorGetZ(p)));
	SetRight(XMFLOAT3(XMVectorGetX(r), XMVectorGetY(r), XMVectorGetZ(r)));
}

void OuterCam::MoveForward(const float d)
{

}

void OuterCam::Reset()
{
	SetPosition(XMFLOAT3(0.0f, hemisphere_radius_outer * cos45, -hemisphere_radius_outer * cos45));
	SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetUp(up);
	SetRight(XMFLOAT3(1.0f, 0.0f, 0.0f));
}