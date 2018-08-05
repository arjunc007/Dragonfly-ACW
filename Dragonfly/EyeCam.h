#pragma once
#include "ICamera.h"
class EyeCam :
	public ICamera
{
public:
	EyeCam(ID3D11Device*, const DirectX::XMFLOAT3&, const DirectX::XMFLOAT3&);
	~EyeCam();

	void MoveUp(const float) override;
	void MoveForward(const float) override;
	void MoveSide(const float) override;

	void Reset() override;
	void Update(const DirectX::XMFLOAT3&, const DirectX::XMFLOAT3&) override;

};

