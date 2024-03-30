#include "Scene.h"

#include "Entity.h"
#include "Renderer/Renderer.h"

Entity Scene::CreateEntity(const std::string& name)
{
    return CreateEntityWithUUID(currentID++, name);
}

Entity Scene::CreateEntityWithUUID(uint32_t uuid, const std::string& name)
{
    Entity entity = Entity(m_Registry.create(), this);
    entity.AddComponent<IDComponent>(uuid);
    entity.AddComponent<TransformComponent>();
    auto& tag = entity.AddComponent<TagComponent>();
    tag.Tag = name.empty() ? "Entity" : name;

    m_EntityMap[uuid] = entity;

    return entity;
}

void Scene::DestroyEntity(Entity entity)
{
    m_EntityMap.erase(entity.GetUUID());
    m_Registry.destroy(entity);
}


Entity Scene::FindEntityByName(std::string_view name)
{
    const auto view = m_Registry.view<TagComponent>();
    for (auto entity : view)
    {
        const TagComponent& tc = view.get<TagComponent>(entity);
        if (tc.Tag == name)
            return {entity, this};
    }
    return {};
}

Entity Scene::GetEntityByUUID(uint32_t uuid)
{
    if (m_EntityMap.find(uuid) != m_EntityMap.end())
        return {m_EntityMap.at(uuid), this};

    return {};
}

void Scene::Step(int frames)
{
    for (const auto& [uuid, entity] : m_EntityMap)
    {
	    Entity e(entity, this);
        e.OnUpdate();
    }
    m_StepFrames = frames;
}

void Scene::SetUpEntity(Entity entity, const std::string& name)
{
	entity.AddComponent<IDComponent>(currentID++);
    entity.AddComponent<TransformComponent>();
    auto& tag = entity.AddComponent<TagComponent>();
    tag.Tag = name.empty() ? "Entity" : name;
}
