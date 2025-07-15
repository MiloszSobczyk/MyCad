#include "CameraSystem.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"

#include <iostream>
#include "entt/entt.hpp"

CameraSystem::CameraSystem(Scene* scene)
	: scene{ scene }
{
	Entity camera = scene->CreateEntity();
	camera.EmplaceComponent<CameraComponent>();
	camera.EmplaceComponent<TranslationComponent>();
	camera.EmplaceComponent<RotationComponent>();
}

void CameraSystem::Update()
{
    std::cout << "CameraSystem Update\n";

    auto view = scene->GetAllEntitiesWith<CameraComponent, TranslationComponent, RotationComponent>();

	for (auto enttEntity : view)
	{
		Entity entity{ enttEntity, scene };

		auto& camera = entity.GetComponent<CameraComponent>();
		auto& translation = entity.GetComponent<TranslationComponent>();
		auto& rotation = entity.GetComponent<RotationComponent>();

		std::cout << camera.cameraLogic->GetViewMatrix(translation.translation, rotation.rotation) << '\n';
	}
}
