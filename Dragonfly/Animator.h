#pragma once
#include "stdafx.h"
#include "Timer.h"
#include <vector>
#include <DirectXMath.h>

class Animator
{
private:
	struct Keyframe
	{
		float time;
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 orientation;
	};

public:
	Animator();
	~Animator();
	void LoadPath(const LPCSTR);
	bool Run(DirectX::XMFLOAT3&, DirectX::XMFLOAT4&, const float&);	
	void Reset() const;

private:
	void GeneratePath(const float& x, const float& y, const float& z);
	void GenerateLeftWingsPath() const;
	void GenerateRightWingsPath() const;
	void GenerateLegsPath() const;

private:
	std::vector<Keyframe> keyframes;
};

