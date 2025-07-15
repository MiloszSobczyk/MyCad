#pragma once

#include "Scene.h"

#include "entt/entt.hpp"

#include <cstdint>
#include <utility>

class Entity 
{
friend Scene;

public:
    Entity(entt::entity entity, Scene* scene);

    template<typename T, typename... Args>
    void EmplaceComponent(Args&&... args) 
    {
        scene->registry->emplace<T>(entityHandle, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    void EmplaceOrReplaceComponent(Args&&... args)
    {
        T& component = scene->registry.emplace_or_replace<T>(entityHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    void RemoveComponent()
    {
        scene->registry.remove<T>(entityHandle);
    }

    template<typename T>
    T& GetComponent() 
    {
        return scene->registry->get<T>(entityHandle);
    }

    template<typename T>
    bool HasComponent() const 
    {
        return scene->registry->all_of<T>(entityHandle);
    }

private:
    entt::entity entityHandle;
    Scene* scene;
};
