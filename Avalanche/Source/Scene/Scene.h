#pragma once

#include <entt/entt.hpp>

class Entity;

class Scene
{
public:
	using Map = std::unordered_map<uint32_t, entt::entity>;

	Scene() = default;
	~Scene() = default;

	Entity CreateEntity(const std::string& name = std::string());
	Entity CreateEntityWithUUID(uint32_t uuid, const std::string& name = std::string());
	template<typename T>
	T CreateEntity(const std::string& name = std::string());
	void DestroyEntity(Entity entity);

	Entity FindEntityByName(std::string_view name);
	Entity GetEntityByUUID(uint32_t uuid);

	bool IsRunning() const { return m_IsRunning; }
	bool IsPaused() const { return m_IsPaused; }

	void SetPaused(bool paused) { m_IsPaused = paused; }

	void Step(int frames = 1);

	template<typename... Components>
	auto GetAllEntitiesWith() const
	{
		return m_Registry.view<Components...>();
	}

	template<typename... Components>
	auto GetAllEntitiesWith()
	{
		return m_Registry.view<Components...>();
	}

private:
	void SetUpEntity(Entity entity, const std::string& name);

	entt::registry m_Registry{};
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	bool m_IsRunning = false;
	bool m_IsPaused = false;
	int m_StepFrames = 0;

	Map m_EntityMap{};

	uint32_t currentID = 0;

	friend class Entity;
	friend class Outliner;
	friend class Renderer;
};

template <typename T>
T Scene::CreateEntity(const std::string& name)
{
	T entity = T(m_Registry.create(), this);
	SetUpEntity(entity, name);

    m_EntityMap[currentID] = entity;

    return entity;
}


