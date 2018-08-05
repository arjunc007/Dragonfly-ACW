#pragma once
#include "ICamera.h"
class InnerCam : public ICamera
{
public:
	explicit InnerCam(ID3D11Device*, const DirectX::XMFLOAT3& position = DirectX::XMFLOAT3(0.0f, cos45 * 10, cos45 * -10)
		, const DirectX::XMFLOAT3& lookAt = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	~InnerCam();

	void MoveUp(const float) override;
	void MoveSide(const float) override;
	void MoveForward(const float) override;

	void Reset() override;

private:
	float m_radius;
};

