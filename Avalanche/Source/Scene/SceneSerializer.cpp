#include "SceneSerializer.h"

#include <yaml-cpp/yaml.h>

#include "Entity.h"

namespace YAML
{
    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };
    
    Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }
}


SceneSerializer::SceneSerializer(Scene* scene)
    : m_Scene(scene)
{
}

static void SerializeEntity(YAML::Emitter& out, Entity entity)
{
    out << YAML::BeginMap; // Entity
    out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

    if (entity.HasComponent<TagComponent>())
    {
        out << YAML::Key << "TagComponent";
        out << YAML::BeginMap; // TagComponent

        auto& tag = entity.GetComponent<TagComponent>().Tag;
        out << YAML::Key << "Tag" << YAML::Value << tag;

        out << YAML::EndMap; // TagComponent
    }
    if (entity.HasComponent<TransformComponent>())
    {
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap; // Transform Component

        auto& tc = entity.GetComponent<TransformComponent>();
        out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
        out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
        out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
        
        out << YAML::EndMap; // Transform Component
    }
    if (entity.HasComponent<PointLightComponent>())
    {
        out << YAML::Key << "PointLightComponent";
        out << YAML::BeginMap; // PointLight Component

        auto& plc = entity.GetComponent<PointLightComponent>();
        out << YAML::Key << "Color" << YAML::Value << plc.Color;

        out << YAML::EndMap; // PointLight Component
    }
    if (entity.HasComponent<BillboardComponent>())
    {
        out << YAML::Key << "BillboardComponent";
        out << YAML::BeginMap; // Billboard Component

        auto& bc = entity.GetComponent<BillboardComponent>().BillboardObject;
        out << YAML::Key << "TexturePath" << YAML::Value << bc->GetTexturePath();

        out << YAML::EndMap; // Billboard Component
    }
    if (entity.HasComponent<StaticMeshComponent>())
    {
        out << YAML::Key << "StaticMeshComponent";
        out << YAML::BeginMap; // StaticMesh Component

        auto& smc = entity.GetComponent<StaticMeshComponent>();
        out << YAML::Key << "Visible" << YAML::Value << smc.Visible;
        out << YAML::Key << "MeshPath" << YAML::Value << smc.StaticMesh->GetMeshPath();
        if (const auto* material = smc.StaticMesh->GetMaterial())
        {
            out << YAML::Key << "TexturePath" << YAML::Value << material->GetAlbedoPath();
        }

        out << YAML::EndMap; // StaticMesh Component
    }

    out << YAML::EndMap; // Entity
}

void SceneSerializer::Serialize(const std::string& filePath)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Untitled";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
    m_Scene->m_Registry.each([&](auto entityID)
    {
        Entity entity = { entityID, m_Scene };
        if (!entity)
            return;

        SerializeEntity(out, entity);
    });
    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream fout(filePath);
    fout << out.c_str();
}

bool SceneSerializer::Deserialize(const std::string& filePath)
{
    YAML::Node data;
    try
    {
        data = YAML::LoadFile(filePath);
    }
    catch (YAML::ParserException e)
    {
        LOG_E("Failed to load .hazel file '{0}'\n     {1}", filePath, e.what())
        return false;
    }

    if (!data["Scene"])
        return false;

    std::string sceneName = data["Scene"].as<std::string>();
    LOG_T("Deserializing scene '{0}'", sceneName)

    if (auto entities = data["Entities"])
    {
        for (auto entity : entities)
        {
            uint32_t uuid = entity["Entity"].as<uint32_t>();

            std::string name;
            if (auto tagComponent = entity["TagComponent"])
                name = tagComponent["Tag"].as<std::string>();

            LOG_T("Deserialized entity with ID = {0}, name = {1}", uuid, name)

            Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

            if (auto transformComponent = entity["TransformComponent"])
            {
                auto& tc = deserializedEntity.GetComponent<TransformComponent>();
                tc.Translation = transformComponent["Translation"].as<glm::vec3>();
                tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                tc.Scale = transformComponent["Scale"].as<glm::vec3>();
            }

            if (auto pointLightComponent = entity["PointLightComponent"])
            {
                auto& pl = deserializedEntity.AddComponent<PointLightComponent>();
                pl.Color = pointLightComponent["Color"].as<glm::vec3>();
            }

            if (auto billboardComponent = entity["BillboardComponent"])
            {
                deserializedEntity.AddComponent<BillboardComponent>(billboardComponent["TexturePath"].as<std::string>());
            }

            if (auto staticMeshComponent = entity["StaticMeshComponent"])
            {
                auto& smc = deserializedEntity.AddComponent<StaticMeshComponent>(staticMeshComponent["MeshPath"].as<std::string>());
                smc.Visible = staticMeshComponent["Visible"].as<bool>();
                const std::string texturePath = staticMeshComponent["TexturePath"].as<std::string>();
                if (!texturePath.empty())
                {
                    if (auto* material = smc.StaticMesh->GetMaterial())
                        material->SetAlbedo(texturePath);
                }
            }
        }
    }
    
    return true;
}
