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
        m_Scene->m_Registry->emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    void EmplaceOrReplaceComponent(Args&&... args)
    {
        T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    void RemoveComponent()
    {
        m_Scene->m_Registry.remove<T>(m_EntityHandle);
    }

    template<typename T>
    T& GetComponent() 
    {
        return m_Scene->m_Registry->get<T>(m_EntityHandle);
    }

    template<typename T>
    bool HasComponent() const 
    {
        return m_Scene->m_Registry->all_of<T>(m_EntityHandle);
    }

private:
    entt::entity m_EntityHandle;
    Scene* m_Scene;
};
