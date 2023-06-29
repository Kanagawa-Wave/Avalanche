#include "Outliner.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Outliner::Outliner(Scene* scene)
{
    SetContext(scene);
}

void Outliner::SetContext(Scene* scene)
{
    m_Context = scene;
}

void Outliner::OnImGuiUpdate()
{
    ImGui::Begin("Outliner");
    m_Context->m_Registry.each([&](auto enttHandle)
    {
        Entity entity(enttHandle, m_Context);
        DrawEntityNode(entity);
    });
    ImGui::End();

    ImGui::Begin("Details");
    if (m_SelectedEntity)
        DrawComponents(m_SelectedEntity);
    ImGui::End();
}

void Outliner::DrawEntityNode(Entity entity)
{
    ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
        ImGuiTreeNodeFlags_OpenOnArrow;

    std::string tag = entity.GetComponent<TagComponent>().Tag;
    bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

    if (ImGui::IsItemClicked())
    {
        m_SelectedEntity = entity;
    }

    if (opened)
    {
        ImGuiTreeNodeFlags flagsChild = ImGuiTreeNodeFlags_OpenOnArrow;
        bool openedChild = ImGui::TreeNodeEx((void*)9817239, flagsChild, tag.c_str());
        if (openedChild)
            ImGui::TreePop();
        ImGui::TreePop();
    }
}

void Outliner::DrawComponents(Entity entity)
{
    if (entity.HasComponent<TagComponent>())
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
        ImGui::Text("Name");
        if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
        {
            tag = std::string(buffer);
        }
        ImGui::Separator();
    }

    if (entity.HasComponent<TransformComponent>())
    {
        if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
                              "Transform"))
        {
            if (ImGui::SmallButton("Reset"))
            {
                entity.RemoveComponent<TransformComponent>();
                entity.AddComponent<TransformComponent>();
            }
            auto& transform = entity.GetComponent<TransformComponent>();
            ImGui::DragFloat3("Position", glm::value_ptr(transform.Translation), 0.1f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(transform.Rotation), 1.f);
            ImGui::DragFloat3("Scale", glm::value_ptr(transform.Scale), 0.1f);
            ImGui::TreePop();
            ImGui::Separator();
        }
    }

    if (entity.HasComponent<MeshComponent>())
    {
        if (ImGui::TreeNodeEx((void*)typeid(MeshComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Mesh"))
        {
            const auto meshPath = entity.GetComponent<MeshComponent>().StaticMesh.GetMeshPath().c_str();
            const auto texturePath = entity.GetComponent<MeshComponent>().StaticMesh.GetTexturePath().c_str();
            ImGui::TextWrapped("Mesh Path: %s", meshPath);
            if (ImGui::Button("Load Mesh"))
            {
                entity.RemoveComponent<MeshComponent>();
                entity.AddComponent<MeshComponent>(FileHelper::OpenFile(".obj")).SetTexture("Content/white.png");
            }
            ImGui::TextWrapped("Texture Path: %s", texturePath);
            if (ImGui::Button("Load Texture"))
            {
                entity.GetComponent<MeshComponent>().SetTexture(FileHelper::OpenFile(".png"));
            }
            ImGui::TreePop();
            ImGui::Separator();
        }
    }
}
