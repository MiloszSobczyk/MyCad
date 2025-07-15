#pragma once

#include "entt/entt.hpp"

#include <cstdint>
#include <utility>

class Entity 
{
public:
    Entity(uint32_t id, entt::registry* registry)
        : registry(registry) 
    {
    }

    template<typename T, typename... Args>
    void AddComponent(Args&&... args) 
    {
        registry->emplace<T>(entityHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    T& GetComponent() 
    {
        return registry->get<T>(entityHandle);
    }

    template<typename T>
    bool HasComponent() const 
    {
        return registry->remove<T>(entityHandle);
    }

private:
    entt::entity entityHandle;
    entt::registry* registry;
};
