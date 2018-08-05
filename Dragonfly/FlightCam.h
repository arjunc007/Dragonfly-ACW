#pragma once
#include "ICamera.h"

class FlightCam :
	public ICamera
{
public:
	FlightCam(ID3D11Device*, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& lookAt);
	~FlightCam();

	void MoveUp(const float) override;
	void MoveSide(const float) override;
	void MoveForward(const float) override;
	void Reset() override;

	void Update(const DirectX::XMFLOAT3&, const DirectX::XMFLOAT3& pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)) override;

private:
	DirectX::XMFLOAT3 m_initialLookAt;
};

