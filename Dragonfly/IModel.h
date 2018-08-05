#pragma once
#include "stdafx.h"
#include "Animator.h"
#include <DirectXMath.h>
#include <d3d11_1.h>

class IModel
{
public:
	IModel();
	virtual ~IModel();

	void SetPosition(const DirectX::XMFLOAT3&);
	void SetRotation(const DirectX::XMFLOAT4&);

	const DirectX::XMFLOAT3& GetPosition() const;
	const DirectX::XMFLOAT4& GetRotation() const;
	const float& GetRadius() const;
	bool IsAnimating() const;
	const float& GetAnimationTime() const;

	virtual bool Initialize(ID3D11Device*, const LPCSTR, const LPCWSTR);

	virtual void Scale(const float, const float, const float) = 0;
	virtual void Rotate(const float, const float, const float) = 0;
	virtual void Move(const float, const float, const float) = 0;

	virtual void Render(ID3D11DeviceContext*) = 0;
	virtual void Reset() = 0;
	virtual void Shutdown() = 0;

protected:
	const DirectX::XMFLOAT3& GetScale() const;
	Animator* GetAnimation() const;

	void SetScale(const DirectX::XMFLOAT3&);
	void SetRadius(const float&);
	void SetAnimation(const Animator* const);
	void SetAnimationTime(const float&);
	void SetAnimating(const bool);

private:
	DirectX::XMFLOAT4 m_rotation;
	DirectX::XMFLOAT3 m_pos;
	DirectX::XMFLOAT3 m_scale;
	float m_radius;
	std::unique_ptr<Animator> animation;
	float animationTime;
	bool animation_running;
};

