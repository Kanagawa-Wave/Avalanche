#pragma once
#include "Scene.h"

class SceneSerializer
{
public:
    SceneSerializer(Scene* scene);
    ~SceneSerializer() = default;

    void Serialize(const std::string& filePath);
    bool Deserialize(const std::string& filePath);

private:
    Scene* m_Scene;
};
