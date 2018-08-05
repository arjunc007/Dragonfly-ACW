#pragma once
class ModelClass
{
public:
	ModelClass();
	~ModelClass();

private:
	std::unique_ptr<DirectX::GeometricPrimitive> m_shape;

};

