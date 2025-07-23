#pragma once
#include "Scene.h"

class SceneSerializer
{
public:
    SceneSerializer(const Scene* scene);
    ~SceneSerializer() = default;

    void Serialize(const std::string& filePath);
    void Deserialize(const std::string& filePath);

private:
    const Scene* m_Scene;
};
