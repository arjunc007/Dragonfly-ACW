#include "stdafx.h"
#include "DirectXManager.h"
#include "Scene.h"

using namespace DirectX;

DirectXManager::DirectXManager()
{
}


DirectXManager::~DirectXManager()
{
}

HRESULT DirectXManager::InitDevice(const HWND ghMainWnd)
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(ghMainWnd, &rc);
	const UINT width = rc.right - rc.left;
	const UINT height = rc.bottom - rc.top;
	
	UINT createDeviceFlags = 0;
	#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif
		
	D3D_FEATURE_LEVEL featureLevel;

	hr = D3D11CreateDevice(
		nullptr, // default adapter
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr, // no software device
		createDeviceFlags,
		nullptr, NULL, // default feature level array
		D3D11_SDK_VERSION,
		&m_pDevice,
		&featureLevel,
		&m_pImmediateContext);
	
	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	//Create IDXGIFactory interface
	IDXGIFactory* dxgiFactory = nullptr;
	IDXGIDevice* dxgiDevice = nullptr;
	hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
	if (SUCCEEDED(hr))
	{
		IDXGIAdapter* dxgiAdapter = nullptr;
		hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));
		if (SUCCEEDED(hr))
		{
			hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));
			dxgiAdapter->Release();
		}
		dxgiDevice->Release();
	}
	if (FAILED(hr))
		return hr;

	//Create swap chain description
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferDesc.Width = width; // use window's client area dims
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = ghMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	// Now, create the swap chain.
	hr = dxgiFactory->CreateSwapChain(m_pDevice, &sd, &m_pSwapChain);

	// Release our acquired COM interfaces (because we are done with them).
	dxgiFactory->Release();

	//Create a render Target View
	ID3D11Texture2D* backBuffer = nullptr;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	if (FAILED(hr))
		return hr;

	hr = m_pDevice->CreateRenderTargetView(backBuffer, nullptr, &m_pRenderTargetView);
	backBuffer->Release();
	if (FAILED(hr))
		return hr;

	//Create depth stencil texture
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	
	hr = m_pDevice->CreateTexture2D(&depthStencilDesc, nullptr, &m_pDepthStencilBuffer);
	if (FAILED(hr))
		return hr; 
	
	//Create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = depthStencilDesc.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &descDSV, &m_pDepthStencilView);
	if (FAILED(hr))
		return hr;

	//Bind views to output merger
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	//Setup the viewport
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_pImmediateContext->RSSetViewports(1, &vp);

	//Setup projection matrix
	m_projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV2, width / static_cast<float>(height), 0.01f, 100.0f);

	//Create the blend state
	D3D11_BLEND_DESC blendStateDescription;
	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	hr = m_pDevice->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if (FAILED(hr))
	{
		return false;
	}

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	hr = m_pDevice->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if (FAILED(hr))
	{
		return false;
	}

	return S_OK;
}

void DirectXManager::CleanupDevice()
{
	if (m_pDevice) m_pDevice->Release();
	if (m_pImmediateContext) m_pImmediateContext->Release();
	if (m_pSwapChain) m_pSwapChain->Release();
	if (m_pRenderTargetView) m_pRenderTargetView->Release();
	if (m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();
	if (m_pDepthStencilView) m_pDepthStencilView->Release();
	if (m_alphaEnableBlendingState) m_alphaEnableBlendingState->Release();
	if (m_alphaDisableBlendingState) m_alphaDisableBlendingState->Release();
}

void DirectXManager::BeginScene(const float& r, const float& g, const float& b, const float& a)
{
	float color[4] = { r, g, b, a };

	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, color);

	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void DirectXManager::EndScene()
{
	m_pSwapChain->Present(0, 0);
}

ID3D11Device* DirectXManager::GetDevice() const
{
	return m_pDevice;
}

ID3D11DeviceContext* DirectXManager::GetDeviceContext() const
{
	return m_pImmediateContext;
}

void DirectXManager::GetProjectionMatrix(XMMATRIX& projectionMatrix) const
{
	projectionMatrix = m_projectionMatrix;
	return;
}

void DirectXManager::EnableAlphaBlending()
{
	m_pImmediateContext->OMSetBlendState(m_alphaEnableBlendingState, nullptr, 0xffffffff);
}

void DirectXManager::DisableAlphaBlending()
{
	m_pImmediateContext->OMSetBlendState(m_alphaDisableBlendingState, nullptr, 0xffffffff);
}