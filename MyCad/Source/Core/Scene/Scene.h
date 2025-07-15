#pragma once

#include "entt/entt.hpp"

class Entity;
class MainSystem;

class Scene
{
friend Entity;
friend MainSystem;

public:
	Scene() = default;
	~Scene() = default;

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	Entity CreateEntity();
	void DestroyEntity(Entity entity);

	template<typename... Components>
	auto GetAllEntitiesWith()
	{
		return registry.view<Components...>();
	}

private:
	entt::registry registry;
};
