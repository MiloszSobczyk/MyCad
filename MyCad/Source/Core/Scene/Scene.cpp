#include "Scene.h"
#include "Entity.h"

Entity Scene::CreateEntity()
{
	Entity entity(registry.create(), this);

	// entt::entity is alias for uint32_t, so we can use it directly
	return entity;
}

void Scene::DestroyEntity(Entity entity)
{
	registry.destroy(entity.entityHandle);
}