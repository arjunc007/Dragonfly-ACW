//Code adapted from http://www.rastertek.com/tutdx11.html

#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>
#include "IModel.h"
#include "Model.h"

class ParticleSystem
{
private:
	struct Particle
	{
		float positionX, positionY, positionZ;
		float red, green, blue;
		float velocity;
		bool active;
	};

	struct ParticleVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT4 color;
	};

public:
	ParticleSystem();
	ParticleSystem(ParticleSystem&);
	ParticleSystem& operator=(const ParticleSystem&);
	~ParticleSystem();

	bool Initialize(ID3D11Device*, const LPCWSTR);
	void Shutdown();
	bool Update(const float, ID3D11DeviceContext*);
	void Render(ID3D11DeviceContext*) const;

	ID3D11ShaderResourceView* GetTexture() const;
	int GetIndexCount() const;
	bool IsActive() const;
	void SetEmitting(const bool);

private:
	
	void ReleaseTexture();

	bool InitializeParticleSystem();
	void ShutdownParticleSystem();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();

	void EmitParticles(const float);
	void UpdateParticles(const float);
	void KillParticles();

	bool UpdateBuffers(ID3D11DeviceContext*);

	void RenderBuffers(ID3D11DeviceContext*) const;
	void Draw(ID3D11DeviceContext*) const;

private:
	std::vector<Particle> m_particleList;
	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;
	int m_maxParticles;
	int m_currentParticleCount;
	float m_accumulatedTime;
	ID3D11ShaderResourceView* m_texture;
	int m_vertexCount, m_indexCount;
	ParticleVertex* m_vertices;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	bool m_isActive, m_isEmitting;
};

