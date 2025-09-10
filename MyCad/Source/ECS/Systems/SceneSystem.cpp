#include "SceneSystem.h"

#include "Algebra.h"
#include "UI/UI.h"
#include "Utils/Logger.h"

#include <iostream>

SceneSystem::SceneSystem()
	: m_Scene{ CreateRef<Scene>() }, 
    m_CameraSystem(m_Scene), 
    m_MeshGenerationSystem(m_Scene),
    m_RenderingSystem(m_Scene),
	m_NotificationSystem(m_Scene),
	m_ShapeManagementSystem(m_Scene)
{
}

void SceneSystem::Update()
{
	UI::ShowFPSWindow();
	UI::DisplayShapeCreator(m_Scene);
	UI::DisplayMergePoints(m_Scene);
	UI::DisplayShapeList(m_Scene);
	UI::DisplaySelectedShapes(m_Scene);

	m_ShapeManagementSystem.Update();
	m_NotificationSystem.Update();
	m_MeshGenerationSystem.Update();
	m_CameraSystem.Update();
    m_RenderingSystem.Update();
}