#include "NotificationSystem.h"

#include "ECS/Components/Components.h"

NotificationSystem::NotificationSystem(Ref<Scene> scene)
	: m_Scene{ scene }
{
}

void NotificationSystem::Update()
{
	for (auto entity : m_Scene->GetAllEntitiesWith<IsDirtyTag>())
	{

	}
}