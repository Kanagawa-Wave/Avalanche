﻿#pragma once

#include "Scene.h"
#include "Components/Components.h"

#include <entt/entt.hpp>

class Entity
{
public:
    Entity() = default;
    Entity(entt::entity handle, Scene* scene);
    Entity(const Entity& other) = default;

    virtual void OnUpdate() {}

    template <typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        ASSERT(!HasComponent<T>(), "Entity already has component!")
        T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        return component;
    }

    template <typename T, typename... Args>
    T& AddOrReplaceComponent(Args&&... args)
    {
        T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
        return component;
    }

    template <typename T>
    T& GetComponent()
    {
        ASSERT(HasComponent<T>(), "Entity does not have component!")
        return m_Scene->m_Registry.get<T>(m_EntityHandle);
    }

    template <typename T>
    bool HasComponent()
    {
        return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
        //return m_Scene->m_Registry.has<T>(m_EntityHandle);
    }

    template <typename T>
    void RemoveComponent()
    {
        ASSERT(HasComponent<T>(), "Entity does not have component!")
        m_Scene->m_Registry.remove<T>(m_EntityHandle);
    }

    operator bool() const { return m_EntityHandle != entt::null; }
    operator entt::entity() const { return m_EntityHandle; }
    operator uint32_t() const { return (uint32_t)m_EntityHandle; }

    uint32_t GetUUID() { return GetComponent<IDComponent>().ID; }
    const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

    bool operator==(const Entity& other) const
    {
        return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
    }

    bool operator!=(const Entity& other) const
    {
        return !(*this == other);
    }

private:
    entt::entity m_EntityHandle{entt::null};
    Scene* m_Scene = nullptr;
};