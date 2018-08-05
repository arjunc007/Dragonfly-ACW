//Code adapted from http://www.rastertek.com/tutdx11.html

#include "ParticleSystem.h"
#include "ModelLoader.h"

using namespace DirectX;

ParticleSystem::ParticleSystem() : m_particleList(0), m_particleDeviationX(0.0f), m_particleDeviationY(0.0f), m_particleDeviationZ(0.0f), m_particleVelocity(0.0f),
m_particleVelocityVariation(0.0f), m_particleSize(0.0f), m_particlesPerSecond(0.0f), m_maxParticles(0), m_currentParticleCount(0), m_accumulatedTime(0.0f), m_texture(nullptr)
, m_vertexCount(0), m_indexCount(0), m_vertices(nullptr), m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_isActive(false), m_isEmitting(true)

{
}

ParticleSystem::~ParticleSystem()
{
	delete[] m_vertices;
}

bool ParticleSystem::Initialize(ID3D11Device* device, const LPCWSTR textureFile)
{
	bool result;

	// Load the texture that is used for the particles.
	const std::unique_ptr<ModelLoader> loader = std::make_unique<ModelLoader>();

	result = loader->LoadTexture(device, textureFile, &m_texture);

	// Initialize the particle system.
	result = InitializeParticleSystem();
	if (!result)
	{
		return false;
	}

	// Create the buffers that will be used to render the particles with.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	m_isActive = true;

	return true;
}

void ParticleSystem::Shutdown()
{
	// Release the buffers.
	ShutdownBuffers();

	// Release the particle system.
	ShutdownParticleSystem();

	// Release the texture used for the particles.
	ReleaseTexture();

	m_isActive = false;

	return;
}

void ParticleSystem::ReleaseTexture()
{
	// Release the texture object.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = nullptr;
	}

	return;
}

bool ParticleSystem::InitializeParticleSystem()
{
	// Set the random deviation of where the particles can be located when emitted.
	m_particleDeviationX = 1.0f;
	m_particleDeviationY = 2.0f;
	m_particleDeviationZ = 1.0f;

	// Set the speed and speed variation of particles.
	m_particleVelocity = 7.0f;
	m_particleVelocityVariation = 0.2f;

	// Set the physical size of the particles.
	m_particleSize = 0.3f;

	// Set the number of particles to emit per second.
	m_particlesPerSecond = 5000.0f;

	// Set the maximum number of particles allowed in the particle system.
	m_maxParticles = 10000;

	// Create and initialize the particle list.
	for (int i = 0; i < m_maxParticles; i++)
	{
		m_particleList.push_back(Particle());
		m_particleList.back().active = false;

	}

	// Initialize the current particle count to zero since none are emitted yet.
	m_currentParticleCount = 0;

	// Clear the initial accumulated time for the particle per second emission rate.
	m_accumulatedTime = 0.0f;

	return true;
}

bool ParticleSystem::Update(const float frameTime, ID3D11DeviceContext* deviceContext)
{
	bool result;

	// Release old particles.
	KillParticles();

	// Emit new particles.
	if(m_isEmitting)
		EmitParticles(frameTime);

	// Update the position of the particles.
	UpdateParticles(frameTime);

	// Update the dynamic vertex buffer with the new position of each particle.
	result = UpdateBuffers(deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void ParticleSystem::Render(ID3D11DeviceContext* deviceContext) const
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	Draw(deviceContext);

	return;
}

void ParticleSystem::ShutdownParticleSystem()
{
	// Release the particle list.
	m_particleList.clear();
	
	return;
}

bool ParticleSystem::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	int i;	
	HRESULT result;

	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = m_maxParticles * 6;

	// Set the maximum number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new ParticleVertex[m_vertexCount];
	if (!m_vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(ParticleVertex) * m_vertexCount));

	// Initialize the index array.
	for (i = 0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(ParticleVertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete[] indices;
	indices = nullptr;

	return true;
}

void ParticleSystem::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}

	return;
}

void ParticleSystem::EmitParticles(const float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;

	float theta, r;


	// Increment the frame time.
	m_accumulatedTime += frameTime;

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// If there are particles to emit then emit one per frame.
	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		// Now generate the randomized particle properties.
		theta = static_cast<float>(rand() % 360);
		r = static_cast<float>(rand() % 200 + 500) / 100.0f;
		positionX = r * sin(XMConvertToRadians(theta)) * m_particleDeviationX;
		positionZ = r * cos(XMConvertToRadians(theta)) * m_particleDeviationZ;
		positionY = ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * m_particleDeviationY;

		velocity = m_particleVelocity + ((static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) * m_particleVelocityVariation;

		//Generate shades of yellow
		red = (static_cast<float>(rand() % 20 + 225)) / 255.0f;// -static_cast<float>(rand())) / RAND_MAX) + 0.5f;
		green = (static_cast<float>(rand() % 90 + 150)) / 255.0f;// -static_cast<float>(rand())) / RAND_MAX) + 0.5f;
		blue = 66.0f / 255.0f;// static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX) + 0.5f;
		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		index = 0;
		found = false;
		while (!found)
		{
			if ((m_particleList[index].active == false) || (m_particleList[index].positionZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}

		// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
		i = m_currentParticleCount;
		j = i - 1;

		while (i != index)
		{
			m_particleList[i].positionX = m_particleList[j].positionX;
			m_particleList[i].positionY = m_particleList[j].positionY;
			m_particleList[i].positionZ = m_particleList[j].positionZ;
			m_particleList[i].red = m_particleList[j].red;
			m_particleList[i].green = m_particleList[j].green;
			m_particleList[i].blue = m_particleList[j].blue;
			m_particleList[i].velocity = m_particleList[j].velocity;
			m_particleList[i].active = m_particleList[j].active;
			i--;
			j--;
		}

		// Now insert it into the particle array in the correct depth order.
		m_particleList[index].positionX = positionX;
		m_particleList[index].positionY = positionY;
		m_particleList[index].positionZ = positionZ;
		m_particleList[index].red = red;
		m_particleList[index].green = green;
		m_particleList[index].blue = blue;
		m_particleList[index].velocity = velocity;
		m_particleList[index].active = true;
	}

	return;
}

void ParticleSystem::UpdateParticles(const float frameTime)
{
	int i;

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for (i = 0; i<m_currentParticleCount; i++)
	{
		m_particleList[i].positionY = m_particleList[i].positionY - (m_particleList[i].velocity * frameTime * 0.001f);
	}

	return;
}

void ParticleSystem::KillParticles()
{
	int i, j;


	// Kill all the particles that have gone below a certain height range.
	for (i = 0; i<m_maxParticles; i++)
	{
		if ((m_particleList[i].active == true) && (m_particleList[i].positionY < -1.0f))
		{
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for (j = i; j<m_maxParticles - 1; j++)
			{
				m_particleList[j].positionX = m_particleList[j + 1].positionX;
				m_particleList[j].positionY = m_particleList[j + 1].positionY;
				m_particleList[j].positionZ = m_particleList[j + 1].positionZ;
				m_particleList[j].red = m_particleList[j + 1].red;
				m_particleList[j].green = m_particleList[j + 1].green;
				m_particleList[j].blue = m_particleList[j + 1].blue;
				m_particleList[j].velocity = m_particleList[j + 1].velocity;
				m_particleList[j].active = m_particleList[j + 1].active;
			}
		}
	}

	return;
}

bool ParticleSystem::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ParticleVertex* verticesPtr;


	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(ParticleVertex) * m_vertexCount));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;

	for (i = 0; i<m_currentParticleCount; i++)
	{
		// Bottom left.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 1.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(0.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top right.
		m_vertices[index].position = XMFLOAT3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = XMFLOAT2(1.0f, 0.0f);
		m_vertices[index].color = XMFLOAT4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	// Lock the vertex buffer.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = static_cast<ParticleVertex*>(mappedResource.pData);

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, static_cast<void*>(m_vertices), (sizeof(ParticleVertex) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

void ParticleSystem::RenderBuffers(ID3D11DeviceContext* deviceContext) const
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(ParticleVertex);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void ParticleSystem::Draw(ID3D11DeviceContext* deviceContext) const
{
	//Set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &m_texture);

	deviceContext->DrawIndexed(m_indexCount, 0, 0);
}

ID3D11ShaderResourceView* ParticleSystem::GetTexture() const
{
	return m_texture;
}

int ParticleSystem::GetIndexCount() const
{
	return m_indexCount;
}

bool ParticleSystem::IsActive() const
{
	return m_isActive;
}

void ParticleSystem::SetEmitting(bool b)
{
	m_isEmitting = b;
}