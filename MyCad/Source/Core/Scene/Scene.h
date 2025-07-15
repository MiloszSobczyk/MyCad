#pragma once

#include <entt/entt.hpp>

class Entity;

class Scene
{
friend Entity;

public:
	Scene() = default;
	~Scene() = default;

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;

	Entity CreateEntity();
	void DestroyEntity(Entity entity);

private:
	entt::registry m_Registry;
};
