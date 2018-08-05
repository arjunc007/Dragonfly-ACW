#pragma once
#include "IModel.h"
#include<vector>

class ModelCollection : public IModel
{
public:
	ModelCollection();
	~ModelCollection();

	void StartAnimation();
	void Scale(const float, const float, const float) override;
	void Rotate(const float, const float, const float) override;
	void Move(const float, const float, const float) override;

	void Update(const float);
	void Render(ID3D11DeviceContext*) override;

	void Reset() override;
	void Shutdown();

	void CreateDragonfly(ID3D11Device*, const float x, const float y, const float z);

private:
	std::vector<std::unique_ptr<IModel>> m_parts;
};

