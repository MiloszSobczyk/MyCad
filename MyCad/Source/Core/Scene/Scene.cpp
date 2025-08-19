#include "Scene.h"

#include "Entity.h"
#include "ECS/Components/TranslationComponent.h"

Scene::Scene()
{
	this->m_Registry.on_construct<TranslationComponent>()
		.connect<&Scene::OnPositionCreated>(this);
}

Scene::~Scene()
{
	this->m_Registry.on_construct<TranslationComponent>()
		.disconnect<&Scene::OnPositionCreated>(this);
}

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

void Scene::OnPositionCreated(entt::registry& registry, entt::entity entity)
{
	auto& translation = registry.get<TranslationComponent>(entity);

	translation.translation.entity = Entity{ entity, this };
}
