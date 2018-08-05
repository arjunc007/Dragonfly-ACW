#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(const float&, const float&, const float&);
	void SetRotation(const float&, const float&, const float&);
	void SetLookAt(const float&, const float&, const float&);
	void SetActive(bool);

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetRotation() const;
	XMFLOAT3 GetLookAt() const;
	bool GetActive() const;

	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	bool m_active;
	XMMATRIX m_viewMatrix;
	float m_positionX, m_positionY, m_positionZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	float m_lookAtX, m_lookAtY, m_lookAtZ;
};

