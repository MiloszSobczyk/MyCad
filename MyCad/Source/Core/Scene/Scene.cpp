#include "Scene.h"
#include "Entity.h"

Entity Scene::CreateEntity()
{
	Entity entity(m_Registry.create(), this);

	// entt::entity is alias for uint32_t, so we can use it directly
	return entity;
}

void Scene::DestroyEntity(Entity entity)
{
	m_Registry.destroy(entity.m_EntityHandle);
}