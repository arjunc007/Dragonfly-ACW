#pragma once
#include<windows.h>
#include <DirectXMath.h>

//Scene
const DirectX::XMFLOAT4 backgroundColor(0.0f, 0.5f, 0.5f, 1.0f);

//Shaders
const LPCWSTR fxFileName = L"Dragonfly.fx";
const LPCSTR lightShaderVS = "VS";
const LPCSTR lightShaderPS = "PS";
const LPCSTR particleShaderVS = "ParticleVertexShader";
const LPCSTR particleShaderPS = "ParticlePixelShader";

//Animation
const float animationLength = 15.0f;

//Models
const LPCSTR cubeFile = "Cube.txt";
const DirectX::XMFLOAT3 dragonfly_initialPos(0.0f, 1.6f, 0.0f);
const DirectX::XMFLOAT4 dragonfly_initialRot(-1.0f, 0.0f, 0.0f, 0.0f);
const LPCWSTR dragonflySkin = L"skin.dds";
const LPCWSTR dragonflyEye = L"eye.dds";
const LPCWSTR dragonflyWings = L"seafloor.dds";
const DirectX::XMFLOAT3 stickPos(0.0f, 0.3f, 0.0f);
const DirectX::XMFLOAT3 stickScale(4.0f, 0.3f, 0.3f);
const LPCWSTR stickTexture = L"wood.dds";
const DirectX::XMFLOAT3 floorScale(50.0f, 0.01f, 50.0f);
const LPCWSTR floorTexture = L"desertfloor.dds";
const LPCWSTR hemisphereTexture = L"sky.dds";

//Light
const DirectX::XMFLOAT4 ambientColor(0.7f, 0.7f, 0.7f, 1.0f);
const DirectX::XMFLOAT4 diffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
const DirectX::XMFLOAT4 specularColor(1.0f, 1.0f, 1.0f, 1.0f);
const float lightPos = 50.0f;
const float specularPower = 32.0f;
const DirectX::XMFLOAT3 lightAttenuation(0.0f, 0.2f, 0.0f);
const float lightRange = 100.0f;
const float lightSpread = 20.0f;

//Cameras
const float hemisphere_radius_inner = 15.0f;
const float hemisphere_radius_outer = 18.0f;
const DirectX::XMFLOAT3 flightCamPos(-10.0f, 2.0f, 10.0f);
const DirectX::XMFLOAT3 up(0.0f, 1.0f, 0.0f);


//Particles
const LPCWSTR particleTexture = L"star.dds";