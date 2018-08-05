#include "InnerCam.h"
#include "Configuration.h"

using namespace DirectX;

InnerCam::InnerCam(ID3D11Device* device, const XMFLOAT3& position, const XMFLOAT3& lookAt) :
	ICamera(device, position, lookAt), m_radius(hemisphere_radius_inner)
{
}


InnerCam::~InnerCam()
{
}

void InnerCam::MoveUp(const float d)
{
	const float y = XMVectorGetX(XMVector3Dot(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), GetPositionXM()));

	const float dist = XMVectorGetX(XMVector3Length(GetPositionXM()));

	if ((y > 1.0f || d > 0.f) && (dist < m_radius || d < 0.0f))
		MoveVertical(d);
}

void InnerCam::MoveSide(const float d)
{
	const float dist = XMVectorGetX(XMVector3Length(GetPositionXM()));

	if (dist < m_radius)
		Strafe(d);
	else
		Strafe(-2.0f*d);

}

void InnerCam::MoveForward(const float d)
{
	const float dist = XMVectorGetX(XMVector3Length(GetPositionXM()));

	if (dist < m_radius && dist > 1.5f)
		Walk(d);
	else
		Walk(-d*2.0f);
}

void InnerCam::Reset()
{
	SetPosition(XMFLOAT3(0.0f, hemisphere_radius_inner * cos45, -hemisphere_radius_inner * cos45));
	SetLookAt(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetUp(up);
	SetRight(XMFLOAT3(1.0f, 0.0f, 0.0f));
}
