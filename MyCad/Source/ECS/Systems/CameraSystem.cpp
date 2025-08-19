#include "CameraSystem.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"
#include "Render/Uniform/UniformManager.h"
#include "Utils/Config.h"

#include <iostream>
#include "entt/entt.hpp"

CameraSystem::CameraSystem(Ref<Scene> scene)
	: m_Scene{ scene }
{
	Entity camera = scene->CreateEntity();
	camera.EmplaceComponent<CameraComponent>();
	auto& tc = camera.EmplaceComponent<TranslationComponent>();
	tc.SetTranslation(Config::InitialCameraPosition);
	camera.EmplaceComponent<RotationComponent>();

	UpdateCameraUniforms();
}

void CameraSystem::Update()
{
	// If changed set matrices
}

void CameraSystem::UpdateCameraUniforms()
{
	auto view = m_Scene->GetAllEntitiesWith<CameraComponent, RotationComponent, TranslationComponent>();

	for (auto entity : view) 
	{
		auto& camera = view.get<CameraComponent>(entity);
		auto& rotation = view.get<RotationComponent>(entity);
		auto& translation = view.get<TranslationComponent>(entity);

		auto viewMatrix = camera.cameraLogic->GetViewMatrix(translation.translation, rotation.rotation);
		auto projectionMatrix = camera.cameraLogic->GetProjectionMatrix(
			static_cast<float>(Config::WindowWidth) / Config::WindowHeight,
			camera.farPlane, camera.nearPlane, camera.fov);

		UniformManager::GetInstance().SetUniformValue("u_viewMatrix", viewMatrix);
		UniformManager::GetInstance().SetUniformValue("u_projectionMatrix", projectionMatrix);
	}
}