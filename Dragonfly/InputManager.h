#pragma once

class Scene;

class InputManager
{
public:
	explicit InputManager(Scene* const scene);
	InputManager(InputManager&);
	InputManager& operator=(const InputManager&);
	~InputManager();

	void ChangeCamera(const int) const;
	//Camera movement
	void MoveCameraLeft();
	void MoveCameraRight();
	void MoveCameraUp();
	void MoveCameraDown();
	void MoveCameraForward();
	void MoveCameraBack();
	//Camera rotation
	void RotateCameraUp();
	void RotateCameraDown();
	void RotateCameraLeft();
	void RotateCameraRight();
	void RunAnimation();
	void SpeedUp();
	void SlowDown();

	void ResetScene();


private:
	Scene* m_scene;
};

