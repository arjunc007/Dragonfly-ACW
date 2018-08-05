#include "IModel.h"

using namespace DirectX;

IModel::IModel() : m_rotation(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)), m_pos(XMFLOAT3(0.f, 0.f, 0.f)), m_scale(XMFLOAT3(1.0f, 1.0f, 1.0f)), m_radius(1.0f),
animation(nullptr), animationTime(0.0f), animation_running(false)
{
}


IModel::~IModel()
{
}

bool IModel::Initialize(ID3D11Device*, LPCSTR, LPCWSTR)
{
	return 0;
}

void IModel::SetPosition(const XMFLOAT3& pos)
{
	m_pos = pos;
	return;
}

void IModel::SetRotation(const XMFLOAT4& rotation)
{
	m_rotation = rotation;
	return;
}

const XMFLOAT3& IModel::GetPosition() const
{
	return m_pos;
}

const XMFLOAT4& IModel::GetRotation() const
{
	return m_rotation;
}

const float& IModel::GetRadius() const
{
	return m_radius;
}

const XMFLOAT3& IModel::GetScale() const
{
	return m_scale;
}

Animator* IModel::GetAnimation() const
{
	return animation.get();
}

const float& IModel::GetAnimationTime() const
{
	return animationTime;
}

bool IModel::IsAnimating() const
{
	return animation_running;
}

void IModel::SetScale(const DirectX::XMFLOAT3& scale)
{
	m_scale = scale;
}

void IModel::SetRadius(const float& radius)
{
	m_radius = radius;
}

void IModel::SetAnimation(const Animator* const anim)
{
	animation.reset(const_cast<Animator*>(anim));
}

void IModel::SetAnimationTime(const float& t)
{
	animationTime = t;
} 

void IModel::SetAnimating(const bool b)
{
	animation_running = b;
}