#include "SceneSystem.h"

#include "Algebra.h"
#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"
#include "ECS/Tags/IsDirtyTag.h"
#include "Platform/OpenGL/OpenGLApi.h"
#include "Render/Buffer/IndexBuffer.h"
#include "Render/Buffer/VertexBuffer.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/Uniform/UniformManager.h"
#include "Utils/Logger.h"

#include <iostream>

SceneSystem::SceneSystem()
	: m_Scene{ CreateRef<Scene>() }, 
    m_CameraSystem(m_Scene), 
    m_MeshGenerationSystem(m_Scene),
    m_RenderingSystem(m_Scene)
{
    Entity entity = m_Scene->CreateEntity();

    entity.EmplaceComponent<TorusComponent>();
	entity.EmplaceComponent<TranslationComponent>();
	entity.EmplaceComponent<RotationComponent>();
	entity.EmplaceComponent<ScalingComponent>();
    entity.EmplaceTag<IsDirtyTag>();

    m_MeshGenerationSystem.Update();
}

void SceneSystem::Update()
{
	m_MeshGenerationSystem.Update();
	m_CameraSystem.Update();
    m_RenderingSystem.Update();
}