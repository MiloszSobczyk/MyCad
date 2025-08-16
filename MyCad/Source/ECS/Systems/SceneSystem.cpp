#include "SceneSystem.h"

#include "Algebra.h"
#include "Creators/ObjectCreator.h"
#include "Utils/Logger.h"
#include "UI/ShapeCreator.h"
#include "UI/ShapeList.h"

#include <iostream>

SceneSystem::SceneSystem()
	: m_Scene{ CreateRef<Scene>() }, 
    m_CameraSystem(m_Scene), 
    m_MeshGenerationSystem(m_Scene),
    m_RenderingSystem(m_Scene)
{
	//ObjectCreator::CreatePoint(m_Scene);
	//ObjectCreator::CreateTorus(m_Scene);
}

void SceneSystem::Update()
{
	UI::DisplayShapeCreator(m_Scene);
	UI::DisplayShapeList(m_Scene);

	m_MeshGenerationSystem.Update();
	m_CameraSystem.Update();
    m_RenderingSystem.Update();
}