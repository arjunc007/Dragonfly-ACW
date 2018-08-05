#include "Scene.h"
#include "OuterCam.h"
#include "InnerCam.h"
#include "FlightCam.h"
#include "EyeCam.h"
#include "Configuration.h"

Scene::Scene() : m_dxManager(nullptr), m_timer(nullptr), m_ground(nullptr), m_camera(0), activeCam(0), m_lights(0), m_shaderManager(nullptr), timeScale(1.0f)
{
}

Scene::~Scene()
{
}

bool Scene::Initialize(const HWND hwnd)
{
	//Start timer
	m_timer = std::make_unique<Timer>();
	m_timer->Start();

	//Initialize DirectX
	m_dxManager = std::make_unique<DirectXManager>();
	if (!m_dxManager)
		return false;

	if (FAILED(m_dxManager->InitDevice(hwnd)))
	{
		m_dxManager->CleanupDevice();
		return false;
	}

	//Create Ground
	m_ground = std::make_unique<Model>();

	m_ground->CreateGround(m_dxManager->GetDevice());

	//Create hemisphere
	m_hemisphere = std::make_unique<Model>();
	m_hemisphere->CreateHemisphere(m_dxManager->GetDevice(), hemisphere_radius_outer);
	
	//Create stick
	m_stick = std::make_unique<Model>();
	
	m_stick->CreateStick(m_dxManager->GetDevice(), stickPos.x, stickPos.y, stickPos.z);

	//Create dragonfly
	m_dragonfly = std::make_unique<ModelCollection>();

	m_dragonfly->CreateDragonfly(m_dxManager->GetDevice(), dragonfly_initialPos.x, dragonfly_initialPos.y, dragonfly_initialPos.z);

	//Create Lights
	for (int i = 0; i < 4; i++)
	{
		m_lights.push_back(std::make_unique<Light>(m_dxManager->GetDevice()));

		m_lights.back()->SetAmbientColor(ambientColor.x, ambientColor.y, ambientColor.z, ambientColor.w);
		m_lights.back()->SetDiffuseColor(diffuseColor.x, diffuseColor.y, diffuseColor.z, diffuseColor.w);
		m_lights.back()->SetPosition(lightPos*sin(i*XM_PI), lightPos, lightPos*cos(i*XM_PI));
		m_lights.back()->SetSpecularColor(specularColor.x, specularColor.y, specularColor.z, specularColor.w);
		m_lights.back()->SetSpecularPower(specularPower);
	}

	//Create Particles
	m_particles = std::make_unique<ParticleSystem>();

	//Initialize cameras
	m_camera.push_back(std::make_unique<OuterCam>(m_dxManager->GetDevice()));
	m_camera.push_back(std::make_unique<InnerCam>(m_dxManager->GetDevice()));
	m_camera.push_back(std::make_unique<FlightCam>(m_dxManager->GetDevice(), flightCamPos, m_dragonfly->GetPosition()));
	XMFLOAT3 front;
	XMStoreFloat3(&front, XMLoadFloat3(&m_dragonfly->GetPosition()) + XMLoadFloat4(&m_dragonfly->GetRotation()) * m_dragonfly->GetRadius());
	m_camera.push_back(std::make_unique<EyeCam>(m_dxManager->GetDevice(), front, m_dragonfly->GetPosition()));

	m_shaderManager = std::make_unique<ShaderManager>();
	if (!m_shaderManager)
		return false;

	if (!m_shaderManager->Initialize(m_dxManager->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

ICamera* Scene::GetActiveCamera()
{
	return m_camera.at(activeCam).get();
}

void Scene::Update()
{
	m_timer->Tick();

	//Update dragonfly
	m_dragonfly->Update(timeScale * m_timer->DeltaTime());
	
	//Update the active camera
	if (activeCam > 1)
	{
		XMFLOAT3 front;
		XMStoreFloat3(&front, XMLoadFloat3(&m_dragonfly->GetPosition()) + XMLoadFloat4(&m_dragonfly->GetRotation()) * m_dragonfly->GetRadius());
		m_camera[activeCam]->Update(m_dragonfly->GetPosition(), front);
	}
	
	if (m_particles->IsActive())
	{
		const float t_anim = m_dragonfly->GetAnimationTime();
		if (t_anim > animationLength*0.14f)
			m_particles->SetEmitting(false);
		m_particles->Update(timeScale * m_timer->DeltaTime() * 50.f, m_dxManager->GetDeviceContext());
	}

	Render();
}

void Scene::ResetTimer()
{
	m_timer->Reset();
	return;
}

void Scene::Shutdown()
{
	if (m_timer)
		m_timer->Reset();

	if (m_dxManager)
	{
		m_dxManager->CleanupDevice();
		m_dxManager = nullptr;
	}

	if (m_shaderManager)
	{
		m_shaderManager->Shutdown();
		m_shaderManager = nullptr;
	}
	
	for (const auto& camera : m_camera)
		camera->Shutdown();
	m_camera.clear();

	if (m_hemisphere)
	{
		m_hemisphere->Shutdown();
		m_hemisphere = nullptr;
	}
	
	if (m_ground)
	{
		m_ground->Shutdown();
		m_ground = nullptr;
	}

	if (m_stick)
	{
		m_stick->Shutdown();
		m_stick = nullptr;
	}

	if (m_particles)
	{
		m_particles->Shutdown();
		m_particles = nullptr;
	}

	if (m_dragonfly)
	{
		m_dragonfly->Shutdown();
		m_dragonfly = nullptr;
	}

	for (const auto& light : m_lights)
		light->Shutdown();
	m_lights.clear();

	return;
}

bool Scene::Render()
{
	m_dxManager->BeginScene(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);

	//Lights
	for(const auto& light : m_lights)
		light->Render(m_dxManager->GetDeviceContext());

	//Camera
	m_camera.at(activeCam)->Render(m_dxManager->GetDeviceContext());

	XMMATRIX viewMatrix, projectionMatrix;
	m_camera.at(activeCam)->GetViewMatrix(viewMatrix);
	m_dxManager->GetProjectionMatrix(projectionMatrix);
	
	if (!m_shaderManager->RenderLightShader(m_dxManager->GetDeviceContext(), viewMatrix, projectionMatrix))
		return false;

	//Models
	m_ground->Render(m_dxManager->GetDeviceContext());
	m_stick->Render(m_dxManager->GetDeviceContext());
	
	m_dragonfly->Render(m_dxManager->GetDeviceContext());

	m_hemisphere->Render(m_dxManager->GetDeviceContext());

	//Particles
	if (m_particles->IsActive())
	{
		m_dxManager->EnableAlphaBlending();

		if (!m_shaderManager->RenderParticleShader(m_dxManager->GetDeviceContext(), XMMatrixIdentity(), viewMatrix, projectionMatrix))
			return false;
		m_particles->Render(m_dxManager->GetDeviceContext());

		m_dxManager->DisableAlphaBlending();
	}
	m_dxManager->EndScene();

	return true;
}

void Scene::Reset()
{
	timeScale = 1.0f;
	m_timer->Reset();
	m_dragonfly->Reset();
	m_camera[activeCam]->Reset();
	m_dragonfly->CreateDragonfly(m_dxManager->GetDevice(), dragonfly_initialPos.x, dragonfly_initialPos.y, dragonfly_initialPos.z);
}

void Scene::SetActiveCam(const int i)
{
	activeCam = i;
}

bool Scene::IsAnimating() const
{
	return m_dragonfly->IsAnimating();
}

void Scene::StartAnimation()
{
	if (m_particles)
		m_particles->Shutdown();

	m_dragonfly->StartAnimation();
	m_particles->Initialize(m_dxManager->GetDevice(), particleTexture);
}

void Scene::ChangeTimeScale(const float dt)
{
	timeScale *= dt;
	return;
}