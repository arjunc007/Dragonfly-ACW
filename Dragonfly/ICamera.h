#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>

const float cos45 = 0.7071f;

class ICamera
{
private:
	struct CameraBuffer
	{
		DirectX::XMFLOAT3 cameraPosition;
		float padding;					//Extra padding to make buffer multiple of 16
	};

public:
	ICamera();
	ICamera(ICamera&);
	ICamera& operator=(const ICamera&);
	virtual ~ICamera();

	void Pitch(const float);
	void RotateY(const float);
	void Strafe(const float);
	void Walk(const float);
	void MoveVertical(const float);
	void Render(ID3D11DeviceContext*);
	void Shutdown();
	
	const DirectX::XMMATRIX& GetViewMatrix(DirectX::XMMATRIX&) const;
	const DirectX::XMFLOAT3& GetPosition() const;
	const DirectX::XMVECTOR& GetPositionXM() const;
	const DirectX::XMFLOAT3& GetLookAt() const;
	const DirectX::XMVECTOR& GetLookAtXM() const;
	const DirectX::XMFLOAT3& GetUp() const;
	const DirectX::XMVECTOR& GetUpXM() const;
	const DirectX::XMFLOAT3& GetRight() const;
	const DirectX::XMVECTOR& GetRightXM() const;

	virtual void MoveSide(const float) = 0;
	virtual void MoveUp(const float) = 0;
	virtual void MoveForward(const float) = 0;
	virtual void Reset() = 0;
	virtual void Update(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& lookAt);

protected:
	ICamera(ID3D11Device*, const DirectX::XMFLOAT3&, const DirectX::XMFLOAT3&);
	void SetPosition(const DirectX::XMFLOAT3&);
	void SetLookAt(const DirectX::XMFLOAT3&);
	void SetUp(const DirectX::XMFLOAT3&);
	void SetRight(const DirectX::XMFLOAT3&);

private:
	DirectX::XMMATRIX m_viewMatrix;
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_lookAt;
	DirectX::XMFLOAT3 m_up;
	DirectX::XMFLOAT3 m_right;
	ID3D11Buffer* m_cameraBuffer;
};

