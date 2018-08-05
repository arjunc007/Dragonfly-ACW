#include "Animator.h"
#include "Configuration.h"

using namespace DirectX;

Animator::Animator() : keyframes(0)
{
}


Animator::~Animator()
{
}

void Animator::LoadPath(const LPCSTR path)
{
	if (path == "Flight")
		GeneratePath(dragonfly_initialPos.x, dragonfly_initialPos.y, dragonfly_initialPos.z);
	else if (path == "LeftWings")
		GenerateLeftWingsPath();
	else if (path == "RightWings")
		GenerateRightWingsPath();
	else if (path == "Legs")
		GenerateLegsPath();
}

bool Animator::Run(XMFLOAT3& position, XMFLOAT4& rotation, const float& t)
{
	//t is before animation started, so return first keyframe
	if (t <= keyframes.front().time)
	{
		const XMVECTOR P = XMLoadFloat3(&keyframes.front().pos);
		const XMVECTOR Q = XMLoadFloat4(&keyframes.front().orientation);
		XMStoreFloat3(&position, P);
		XMStoreFloat4(&rotation, Q);

		return false;
	}
	//t is after animation ended, so return last keyframe
	else if (t >= keyframes.back().time)
	{
		const XMVECTOR P = XMLoadFloat3(&keyframes.back().pos);
		const XMVECTOR Q = XMLoadFloat4(&keyframes.back().orientation);
		XMStoreFloat3(&position, P);
		XMStoreFloat4(&rotation, Q);

		return false;
	}
	//t is between two keyframes, so interpolate
	else 
	{
		for(UINT i = 0; i < keyframes.size(); i++)
			if (t >= keyframes[i].time && t <= keyframes[i + 1].time)
			{
				const float lerpPercent = (t - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
				
				const XMVECTOR p0 = XMLoadFloat3(&keyframes[i].pos);
				const XMVECTOR p1 = XMLoadFloat3(&keyframes[i+1].pos);

				const XMVECTOR q0 = XMLoadFloat4(&keyframes[i].orientation);
				const XMVECTOR q1 = XMLoadFloat4(&keyframes[i+1].orientation);

				const XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
				const XMVECTOR Q = XMQuaternionSlerp(q0, q1, lerpPercent);

				XMStoreFloat3(&position, P);
				XMStoreFloat4(&rotation, Q);

				return true;
			}
	}

	return true;
}

void Animator::GeneratePath(const float& x, const float& y, const float& z)
{
	Keyframe k;

	//t = 0;
	k.time = 0.0f;
	k.pos = XMFLOAT3(x, y, z);
	k.orientation = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	keyframes.push_back(k);

	k.time = 0.07f * animationLength;
	k.pos = XMFLOAT3(0.0f, 5.0f, 0.0f);
	k.orientation = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	keyframes.push_back(k);

	k.time = 0.127f * animationLength;
	k.pos = XMFLOAT3(-8.0f, 8.0f, 0.0f);
	keyframes.push_back(k);

	for (float i = 10; i < 540; i+=10)
	{
		k.time += 0.014f * animationLength;
		k.pos = XMFLOAT3(-8.0f*cos(XMConvertToRadians(i)), 8.0f + sin(XMConvertToRadians(i))*2.0f, 8.0f*sin(XMConvertToRadians(i)));
		keyframes.push_back(k);
	}
	//t = 12.4
	k.time += 0.056f * animationLength;
	k.pos = XMFLOAT3(0.0, 5.0f, 0.0f);
	keyframes.push_back(k);
	
	//t = 13.2
	k.time += 0.07f * animationLength;
	k.pos = XMFLOAT3(x, y, z);
	keyframes.push_back(k);
}

void Animator::GenerateLegsPath() const
{
	/*m_pos.push_back(XMFLOAT3(0.f, 0.0f, 0.0f));*/
}

void Animator::GenerateLeftWingsPath() const
{
	/*m_pos.push_back(XMFLOAT3(0.f, 0.0f, 0.0f));*/
}

void Animator::GenerateRightWingsPath() const
{

}

void Animator::Reset() const
{
	/*m_currentPos = 0;*/
}
