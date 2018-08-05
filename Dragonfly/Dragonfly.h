#pragma once
class Dragonfly
{
private:

public:
	Dragonfly();
	Dragonfly(ID3D11DeviceContext*);
	~Dragonfly();

	void Update(const DirectX::XMMATRIX&);
	void Draw(const DirectX::XMMATRIX&, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&);
	DirectX::XMFLOAT3 GetPos() const;

private:
	float m_posX, m_posY, m_posZ;
	std::unique_ptr<DirectX::GeometricPrimitive> m_head;
	std::unique_ptr<DirectX::GeometricPrimitive> m_thorax;
	std::unique_ptr<DirectX::GeometricPrimitive> m_abdomen;
	std::unique_ptr<DirectX::GeometricPrimitive> m_tail;
	std::unique_ptr<DirectX::GeometricPrimitive> m_legs;
	std::unique_ptr<DirectX::GeometricPrimitive> m_wings;
	DirectX::XMMATRIX m_headMatrix;
	DirectX::XMMATRIX m_thoraxMatrix;
	DirectX::XMMATRIX m_abdomenMatrix;
	DirectX::XMMATRIX m_tailMatrix;
	std::vector<DirectX::XMMATRIX> m_legsMatrix;
	std::vector<DirectX::XMMATRIX> m_wingsMatrix;
};

