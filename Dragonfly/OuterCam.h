#pragma once
#include "ICamera.h"
#include "Configuration.h"

class OuterCam : public ICamera
{
public:
	explicit OuterCam( ID3D11Device*, const DirectX::XMFLOAT3& position = DirectX::XMFLOAT3(0.0f, cos45 * 20, cos45 * -20)
		, const DirectX::XMFLOAT3& lookAt = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	~OuterCam();

	void MoveUp(const float) override;
	void MoveSide(const float) override;
	void MoveForward(const float) override;

	void Reset() override;

private:
	float m_radius;
};

