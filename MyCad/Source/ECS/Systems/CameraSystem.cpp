#include "CameraSystem.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"
#include "Render/Uniform/UniformManager.h"

#include <iostream>
#include "entt/entt.hpp"

CameraSystem::CameraSystem(Ref<Scene> scene)
	: m_Scene{ scene }
{
	Entity camera = scene->CreateEntity();
	camera.EmplaceComponent<CameraComponent>();
	camera.EmplaceComponent<TranslationComponent>();
	camera.EmplaceComponent<RotationComponent>();

	UniformManager::GetInstance().RegisterUniform("u_projectionMatrix", Algebra::Matrix4::Identity());
	UniformManager::GetInstance().RegisterUniform("u_viewMatrix", Algebra::Matrix4::Identity());
}

void CameraSystem::Update()
{
}