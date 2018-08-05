#pragma once
#include "stdafx.h"
#include<d3d11_1.h>
#include<DirectXMath.h>
#include "ModelLoader.h"
#include "SimpleVertex.h"
#include "IModel.h"

class Model : public IModel
{
private:
	struct ModelBuffer
	{
		DirectX::XMMATRIX worldMatrix;
	};

public:
	Model();
	Model(Model&);
	~Model();

	bool Initialize(ID3D11Device*, const LPCSTR, const LPCWSTR) override;
	void Shutdown();
	void Render(ID3D11DeviceContext*) override;

	int GetVertexCount() const;
	int GetIndexCount() const;
	ID3D11ShaderResourceView* GetTexture() const;

	void Scale(const float, const float, const float) override;
	void Rotate(const float, const float, const float) override;
	void Move(const float, const float, const float) override;

	void CreateStick(ID3D11Device*, const float, const float, const float);
	void CreateGround(ID3D11Device*);
	void CreateHemisphere(ID3D11Device*, const float);

	void Reset() override;

private:
	Model& operator=(const Model&) = delete;

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool RenderBuffers(ID3D11DeviceContext*) const;
	void Draw(ID3D11DeviceContext*) const;
	void ReleaseModel();

private:
	ID3D11Buffer* m_vertexBuffer, *m_indexBuffer;
	ID3D11Buffer* m_modelBuffer;
	int m_vertexCount, m_indexCount;
	ID3D11ShaderResourceView* m_texture;
	std::vector<SimpleVertex> m_vertices;
};

