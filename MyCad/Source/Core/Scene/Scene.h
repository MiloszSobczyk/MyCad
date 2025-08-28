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

    template<typename... Include>
    std::vector<Entity> GetAllEntitiesWith()
    {
        std::vector<Entity> entities;
        auto view = m_Registry.view<Include...>();

        for (auto entity : view)
            entities.emplace_back(entity, this);

        return entities;
    }

    template<typename... Include, typename... Exclude>
    std::vector<Entity> GetAllEntitiesWith(entt::exclude_t<Exclude...> exclude)
    {
        std::vector<Entity> entities;
        auto view = m_Registry.view<Include...>(exclude);

        for (auto entity : view)
            entities.emplace_back(entity, this);

        return entities;
    }

    template<typename... Include, typename... Exclude>
    std::vector<Entity> GetAllEntitiesWithAny(entt::exclude_t<Exclude...> exclude)
    {
        std::vector<Entity> entities;
        std::unordered_set<entt::entity> seen;

        ([&] {
            auto view = m_Registry.view<Include>(exclude);
            for (auto entity : view) {
                if (seen.insert(entity).second) {
                    entities.emplace_back(entity, this);
                }
            }
            }(), ...);

        return entities;
    }

	void OnPositionCreated(entt::registry& registry, entt::entity entity);

private:
	entt::registry m_Registry;
};
