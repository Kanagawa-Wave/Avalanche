#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Billboard.h"
#include "Camera.h"
#include "Mesh.h"

struct TagComponent
{
    std::string Tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;

    TagComponent(const std::string& tag)
        : Tag(tag)
    {
    }
};

struct IDComponent
{
    uint32_t ID;

    IDComponent() = default;
    IDComponent(const IDComponent&) = default;

    IDComponent(uint32_t UUID) : ID(UUID)
    {
    }
};

struct TransformComponent
{
    glm::vec3 Translation = {0.0f, 0.0f, 0.0f};
    // Stored as degrees
    glm::vec3 Rotation = {0.0f, 0.0f, 0.0f};
    glm::vec3 Scale = {1.0f, 1.0f, 1.0f};

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;

    TransformComponent(const glm::vec3& translation)
        : Translation(translation)
    {
    }

    TransformComponent(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
        : Translation(translation), Rotation(rotation), Scale(scale)
    {
    }

    glm::mat4 GetTransform() const
    {
        glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

        return glm::translate(glm::mat4(1.0f), Translation)
            * rotation
            * glm::scale(glm::mat4(1.0f), Scale);
    }

    glm::mat4 GetModelMat() const
    {
        // convert to radians
        const glm::vec3 rotation = glm::radians(Rotation);
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);
        return glm::mat4{
            {
                Scale.x * (c1 * c3 + s1 * s2 * s3),
                Scale.x * (c2 * s3),
                Scale.x * (c1 * s2 * s3 - c3 * s1),
                0.0f,
            },
            {
                Scale.y * (c3 * s1 * s2 - c1 * s3),
                Scale.y * (c2 * c3),
                Scale.y * (c1 * c3 * s2 + s1 * s3),
                0.0f,
            },
            {
                Scale.z * (c2 * s1),
                Scale.z * (-s2),
                Scale.z * (c1 * c2),
                0.0f,
            },
            {Translation.x, Translation.y, Translation.z, 1.0f}
        };
    }

    glm::mat4 GetNormalMat() const
    {
        // convert to radians
        const glm::vec3 rotation = glm::radians(Rotation);
        const float c3 = glm::cos(rotation.z);
        const float s3 = glm::sin(rotation.z);
        const float c2 = glm::cos(rotation.x);
        const float s2 = glm::sin(rotation.x);
        const float c1 = glm::cos(rotation.y);
        const float s1 = glm::sin(rotation.y);

        const glm::vec3 inverseScale = 1.0f / Scale;

        return glm::mat3{
            {
                inverseScale.x * (c1 * c3 + s1 * s2 * s3),
                inverseScale.x * (c2 * s3),
                inverseScale.x * (c1 * s2 * s3 - c3 * s1),
            },
            {
                inverseScale.y * (c3 * s1 * s2 - c1 * s3),
                inverseScale.y * (c2 * c3),
                inverseScale.y * (c1 * c3 * s2 + s1 * s3),
            },
            {
                inverseScale.z * (c2 * s1),
                inverseScale.z * (-s2),
                inverseScale.z * (c1 * c2),
            },
        };
    }
};

struct CameraComponent
{
    Camera AttachedCamera{};

    CameraComponent() = default;
    CameraComponent(const CameraComponent&) = default;
};

struct PointLightComponent
{
    glm::vec3 Color{1.f};

    PointLightComponent() = default;
    PointLightComponent(const PointLightComponent&) = default;
};

struct DirectionalLightComponent
{
    glm::vec3 Color{1.f};

    DirectionalLightComponent() = default;
    DirectionalLightComponent(const DirectionalLightComponent&) = default;
};

struct BillboardComponent
{
    std::shared_ptr<Billboard> BillboardObject = nullptr;

    BillboardComponent() = default;
    BillboardComponent(const BillboardComponent&) = default;

    BillboardComponent(const std::string& path)
    {
        BillboardObject = std::make_shared<Billboard>(1.f, path);
    }
};

struct ShadowMapComponent
{
	uint32_t ShadowMapWidth = 1024;
    uint32_t ShadowMapHeight = 1024;
    glm::mat4 Projection, View;

	ShadowMapComponent() = default;
    ShadowMapComponent(const ShadowMapComponent&) = default;
};

struct StaticMeshComponent
{
    std::shared_ptr<Mesh> StaticMesh = nullptr;
    bool Visible = false;
    
    StaticMeshComponent() = default;
    StaticMeshComponent(const StaticMeshComponent&) = default;
    
    StaticMeshComponent(const std::string& path)
    {
        StaticMesh = std::make_shared<Mesh>(path);
        Visible = true;
    }
    StaticMeshComponent(std::shared_ptr<Mesh> mesh)
        : StaticMesh(mesh), Visible(true) {}
};

template <typename... Component>
struct ComponentGroup
{
};

using AllComponents =
    ComponentGroup<IDComponent, TagComponent, TransformComponent,
                   CameraComponent, StaticMeshComponent, PointLightComponent,
                   BillboardComponent>;
