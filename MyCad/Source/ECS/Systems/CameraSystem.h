#pragma once

class Scene;

class CameraSystem
{
public:
	CameraSystem(Scene* scene);
	~CameraSystem() = default;

	CameraSystem(const CameraSystem&) = delete;
	CameraSystem& operator=(const CameraSystem&) = delete;

	void Update();
	
private:
	Scene* scene = nullptr;
};