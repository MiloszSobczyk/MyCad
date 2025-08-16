#include "SceneSystem.h"

#include "Algebra.h"
#include "Creators/ShapeCreator.h"
#include "Utils/Logger.h"

#include <iostream>

SceneSystem::SceneSystem()
	: m_Scene{ CreateRef<Scene>() }, 
    m_CameraSystem(m_Scene), 
    m_MeshGenerationSystem(m_Scene),
    m_RenderingSystem(m_Scene)
{
	ShapeCreator::CreatePoint(m_Scene);
	ShapeCreator::CreateTorus(m_Scene);
}

void SceneSystem::Update()
{
	m_MeshGenerationSystem.Update();
	m_CameraSystem.Update();
    m_RenderingSystem.Update();
}