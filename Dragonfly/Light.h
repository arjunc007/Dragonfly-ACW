#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>

const int NUM_LIGHTS = 4;

class Light
{
private:
	struct LightPositionBuffer
	{
		DirectX::XMFLOAT3 lightPosition[NUM_LIGHTS];
		DirectX::XMFLOAT3 lightFocus[NUM_LIGHTS];
	};

	struct LightBuffer
	{
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT4 specularColor;
		float specularPower;
		DirectX::XMFLOAT3 attenuation;
		float range;
		float cone;
		DirectX::XMFLOAT2 padding;
	};

public:
	explicit Light(ID3D11Device*);
	Light(const Light&);
	Light& operator=(const Light&);
	~Light();

	void SetAmbientColor(const float&, const float&, const float&, const float&);
	void SetDiffuseColor(const float&, const float&, const float&, const float&);
	void SetPosition(const float&, const float&, const float&);
	void SetSpecularColor(const float&, const float&, const float&, const float&);
	void SetSpecularPower(const float&);
	void SetFocus(const float, const float, const float);
	void Shutdown();

	const DirectX::XMFLOAT4& GetAmbientColor() const;
	const DirectX::XMFLOAT4& GetDiffuseColor() const;
	const DirectX::XMFLOAT3& GetPosition() const;
	const DirectX::XMFLOAT4& GetSpecularColor() const;
	const float& GetSpecularPower() const;

	void Render(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_lightPositionBuffer;
	DirectX::XMFLOAT4 m_ambientColor;
	DirectX::XMFLOAT4 m_diffuseColor;
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT4 m_specularColor;
	float m_specularPower;
	int lightNum;
	static int lightIndex;
	DirectX::XMFLOAT3 m_focus;
};

