#include "CameraSystem.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"

#include <iostream>
#include "entt/entt.hpp"

CameraSystem::CameraSystem(Ref<Scene> scene)
	: m_Scene{ scene }
{
	Entity camera = scene->CreateEntity();
	camera.EmplaceComponent<CameraComponent>();
	camera.EmplaceComponent<TranslationComponent>();
	camera.EmplaceComponent<RotationComponent>();
}

void CameraSystem::Update()
{
}