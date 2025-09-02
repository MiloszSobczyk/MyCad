#include "NotificationSystem.h"

#include "ECS/Components/Components.h"

NotificationSystem::NotificationSystem(Ref<Scene> scene)
	: m_Scene{ scene }
{
}

void NotificationSystem::Update()
{
	for (auto e : m_Scene->GetAllEntitiesWith<IsNotifiedTag>())
	{
		if (e.HasComponent<NotificationComponent>())
		{
			auto& nc = e.GetComponent<NotificationComponent>();
			for (auto toNotify : nc.toNotifys)
			{
				Entity targetEntity{ toNotify, m_Scene.get() };
				targetEntity.EmplaceComponent<DirtyFromComponent>();
			}
		}
		e.RemoveComponent<IsNotifiedTag>();
	}
}