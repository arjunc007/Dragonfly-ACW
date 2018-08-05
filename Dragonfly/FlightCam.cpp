#include "FlightCam.h"
#include "Configuration.h"

using namespace DirectX;

FlightCam::FlightCam(ID3D11Device* device, const XMFLOAT3& position, const XMFLOAT3& lookAt) :
	ICamera(device, position, lookAt), m_initialLookAt(lookAt)
{
}


FlightCam::~FlightCam()
{
}

void FlightCam::MoveUp(const float d)
{}

void FlightCam::MoveSide(const float d)
{}

void FlightCam::MoveForward(const float d)
{}

void FlightCam::Update(const XMFLOAT3& lookAt, const XMFLOAT3& pos)
{
	SetLookAt(lookAt);
}

void FlightCam::Reset()
{
	SetPosition(flightCamPos);
	SetLookAt(m_initialLookAt);
	SetUp(up);
}
