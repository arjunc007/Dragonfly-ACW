#pragma once
#ifndef SIMPLEVERTEX_H
#define SIMPLEVERTEX_H

#include<DirectXMath.h>

struct SimpleVertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 Texture;
};

#endif // !SIMPLEVERTEX_H
