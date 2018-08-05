#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>

using namespace DirectX;

class Scene;

class DirectXManager
{
public:
	DirectXManager();
	DirectXManager(DirectXManager&);
	DirectXManager& operator=(const DirectXManager&);
	~DirectXManager();

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;
	void GetProjectionMatrix(XMMATRIX&) const;

	HRESULT InitDevice(const HWND ghMainWnd);
	void BeginScene(const float& r, const float& g, const float& b, const float& a);
	void EndScene();
	void CleanupDevice();
	void EnableAlphaBlending();
	void DisableAlphaBlending();

private:
	XMMATRIX m_projectionMatrix;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
	ID3D11Texture2D* m_pDepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
	ID3D11BlendState* m_alphaEnableBlendingState = nullptr;
	ID3D11BlendState* m_alphaDisableBlendingState = nullptr;
};

