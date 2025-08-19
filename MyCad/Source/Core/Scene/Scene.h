#pragma once

#include "entt/entt.hpp"

class Entity;
class MainSystem;

class Scene
{
friend Entity;
friend MainSystem;

public:
	Scene();
	~Scene();

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	Entity CreateEntity();
	void DestroyEntity(Entity entity);

	template<typename... Components>
	auto GetAllEntitiesWith()
	{
		return m_Registry.view<Components...>();
	}

	void OnPositionCreated(entt::registry& registry, entt::entity entity);

private:
	entt::registry m_Registry;
};
