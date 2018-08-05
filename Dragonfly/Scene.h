#pragma once
#include "DirectXManager.h"
#include "Model.h"
#include "ShaderManager.h"
#include "Light.h"
#include "Timer.h"
#include "ICamera.h"
#include "InputManager.h"
#include "ModelCollection.h"
#include "ParticleSystem.h"
#include <vector>

class Scene
{
public:
	Scene();
	Scene(const Scene&);
	~Scene();

	bool Initialize(HWND);
	void Update();
	bool Render();
	void Shutdown();

	ICamera* GetActiveCamera();
	void SetActiveCam(const int i);

	bool IsAnimating() const;
	void StartAnimation();
	void ChangeTimeScale(const float);

	void ResetTimer();
	void Reset();

private:
	std::unique_ptr<DirectXManager> m_dxManager;
	std::unique_ptr<Timer> m_timer;
	std::unique_ptr<Model> m_ground;
	std::unique_ptr<Model> m_stick;
	std::unique_ptr<Model> m_hemisphere;
	std::unique_ptr<ModelCollection> m_dragonfly;
	std::vector<std::unique_ptr<ICamera>> m_camera;
	int activeCam;
	std::vector<std::unique_ptr<Light>> m_lights;
	std::unique_ptr<ShaderManager> m_shaderManager;
	std::unique_ptr<ParticleSystem> m_particles;
	float timeScale;

};

