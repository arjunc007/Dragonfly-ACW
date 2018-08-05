#include "InputManager.h"
#include "Scene.h"

using namespace DirectX;

const float deg2rad = XM_PI / 180;

InputManager::InputManager(Scene* const scene) : m_scene(scene)
{
}


InputManager::~InputManager()
{
}

void InputManager::ResetScene()
{
	m_scene->Reset();
}

void InputManager::ChangeCamera(const int i)  const
{
	m_scene->SetActiveCam(i);
}

void InputManager::MoveCameraLeft()
{
	m_scene->GetActiveCamera()->MoveSide(-1.0f);
}
void InputManager::MoveCameraRight()
{
	m_scene->GetActiveCamera()->MoveSide(1.0f);
}
void InputManager::MoveCameraUp()
{
	m_scene->GetActiveCamera()->MoveUp(1.0f);
}
void InputManager::MoveCameraDown()
{
	m_scene->GetActiveCamera()->MoveUp(-1.0f);
}
void InputManager::MoveCameraForward() 
{
	m_scene->GetActiveCamera()->MoveForward(0.02f);
}

void InputManager::MoveCameraBack()
{
	m_scene->GetActiveCamera()->MoveForward(-0.02f);
}

void InputManager::RotateCameraUp()
{
	m_scene->GetActiveCamera()->Pitch(5.0f * deg2rad);
}

void InputManager::RotateCameraDown()
{
	m_scene->GetActiveCamera()->Pitch(-5.0f * deg2rad);
}

void InputManager::RotateCameraLeft()
{
	m_scene->GetActiveCamera()->RotateY(-5.0f * deg2rad);
}

void InputManager::RotateCameraRight()
{
	m_scene->GetActiveCamera()->RotateY(5.0f * deg2rad);
}

void InputManager::RunAnimation()
{
	if(!m_scene->IsAnimating())
		m_scene->StartAnimation();
}

void InputManager::SpeedUp()
{
	m_scene->ChangeTimeScale(2.0f);
}

void InputManager::SlowDown()
{
	m_scene->ChangeTimeScale(0.5f);
}