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
	ImGui::BeginTabBar("OutlinerTabBar");
	if (ImGui::TabItemButton("+"))
	{
		m_SelectedEntity = m_Context->CreateEntity();
	}
	if (ImGui::TabItemButton("-") && m_SelectedEntity)
	{
		m_Context->DestroyEntity(m_SelectedEntity);
		m_SelectedEntity = Entity();
	}
	ImGui::EndTabBar();

		m_Context->m_Registry.each([&](auto enttHandle)
		{
			Entity entity(enttHandle, m_Context);
			DrawEntityNode(entity);
		});
	ImGui::End();

	ImGui::Begin("Details");
	if (m_SelectedEntity)
	{
		ImGui::BeginTabBar("OutlinerTabBar");
		if (ImGui::TabItemButton("Add Component"))
		{
			ImGui::OpenPopup("OutlinerPopup");
		}
	
		if (ImGui::BeginPopup("OutlinerPopup"))
		{
			if (ImGui::MenuItem("PointLight Component"))
			{
				m_SelectedEntity.AddOrReplaceComponent<PointLightComponent>();
			}
			if (ImGui::MenuItem("StaticMesh Component"))
			{
				m_SelectedEntity.AddOrReplaceComponent<StaticMeshComponent>();
			}
			if (ImGui::MenuItem("Billboard Component"))
			{
				m_SelectedEntity.AddOrReplaceComponent<BillboardComponent>();
			}
			ImGui::EndPopup();
		}
		ImGui::EndTabBar();
		DrawComponents(m_SelectedEntity);
	}
	ImGui::End();
}

void Outliner::DrawEntityNode(Entity entity)
{
	ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0); // | ImGuiTreeNodeFlags_OpenOnArrow;
	std::string tag = entity.GetComponent<TagComponent>().Tag;
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

	if (ImGui::IsItemClicked())
	{
		m_SelectedEntity = entity;
	}

	// if (opened)
	// {
	//     ImGuiTreeNodeFlags flagsChild = ImGuiTreeNodeFlags_OpenOnArrow;
	//     bool openedChild = ImGui::TreeNodeEx((void*)9817239, flagsChild, tag.c_str());
	//     if (openedChild)
	//         ImGui::TreePop();
	//     ImGui::TreePop();
	// }
	if (opened)
		ImGui::TreePop();
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
			auto& transform = entity.GetComponent<TransformComponent>();
			
			if (ImGui::SmallButton("Reset##2"))
			{
				transform.Translation = glm::vec3(0, 0, 0);
			}
			ImGui::SameLine();
			ImGui::DragFloat3("Position", glm::value_ptr(transform.Translation), 0.1f);
			
			if (ImGui::SmallButton("Reset##3"))
			{
				transform.Rotation = glm::vec3(0, 0, 0);
			}
			ImGui::SameLine();
			ImGui::DragFloat3("Rotation", glm::value_ptr(transform.Rotation), 1.f);
			
			if (ImGui::SmallButton("Reset##4"))
			{
				transform.Scale = glm::vec3(1, 1, 1);
			}
			ImGui::SameLine();
			ImGui::DragFloat3("Scale", glm::value_ptr(transform.Scale), 0.1f);
			
			ImGui::TreePop();
			ImGui::Separator();
		}
	}

	if (entity.HasComponent<PointLightComponent>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(PointLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen,
			"Point Light"))
		{
			ImGui::SameLine();
			if (ImGui::SmallButton("Remove"))
			{
				entity.RemoveComponent<PointLightComponent>();
			}
			if (entity.HasComponent<PointLightComponent>())
			{
				auto& pointLight = entity.GetComponent<PointLightComponent>();
				ImGui::ColorPicker3("Color", glm::value_ptr(pointLight.Color));
			}
			ImGui::TreePop();
			ImGui::Separator();
		}
	}

	if (entity.HasComponent<StaticMeshComponent>())
	{
	    if (ImGui::TreeNodeEx((void*)typeid(StaticMeshComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Mesh"))
	    {
	    	ImGui::SameLine();
	    	if (ImGui::SmallButton("Remove"))
	    	{
	    		entity.RemoveComponent<StaticMeshComponent>();
	    	}
	    	if (entity.HasComponent<StaticMeshComponent>())
	    	{
	    		auto& mesh = entity.GetComponent<StaticMeshComponent>();

	    		if (mesh.StaticMesh)
	    		{
	    			ImGui::Checkbox("Visible", &mesh.Visible);
	    		}
	    	
	    		std::string meshPath = "NULL", texturePath = "NULL";
	    		if (mesh.StaticMesh)
	    		{
	    			meshPath = entity.GetComponent<StaticMeshComponent>().StaticMesh->GetMeshPath();
	    			texturePath = entity.GetComponent<StaticMeshComponent>().StaticMesh->GetTexturePath();
	    		}
	    		ImGui::TextWrapped("Mesh: %s", meshPath.c_str());
	    		if (ImGui::Button("Load Mesh"))
	    		{
	    			const std::string path = FileHelper::OpenFile("3D Model (*.obj)\0*.obj\0");
	    			if (!path.empty())
	    			{
	    				mesh.StaticMesh = std::make_shared<Mesh>(path);
	    				mesh.Visible = true;
	    				mesh.StaticMesh->SetTexture("Content/white.png");
	    			}
	    		}
	    		ImGui::TextWrapped("Texture: %s", texturePath.c_str());
	    		if (ImGui::Button("Load Texture"))
	    		{
	    			const std::string path = FileHelper::OpenFile("Image File (*.png)\0*.png\0");
	    			if (!path.empty() && mesh.StaticMesh)
	    			{
	    				mesh.StaticMesh->SetTexture(path);
	    			}
	    		}
	    	}
	        ImGui::TreePop();
	        ImGui::Separator();
	    }
	}
}
