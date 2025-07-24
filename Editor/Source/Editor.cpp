#include "Editor.h"

#include <imgui.h>

#include "Panels/Outliner.h"
#include "Scene/SceneSerializer.h"

Application* CreateApplication()
{
	return new Editor();
}

Editor::Editor()
{
	m_ViewportExtent = m_Window->GetExtent();
	m_EditorCamera = std::make_unique<Camera>(m_ViewportExtent.width, m_ViewportExtent.height, 30.0f, 0.1f, 1000.0f);

	m_Renderer = std::make_unique<Renderer>(m_Window.get(), m_ViewportExtent);

	m_Scene = std::make_unique<Scene>();
	m_ScenePath = "";

	m_Outliner = std::make_unique<Outliner>(m_Scene.get());
	
	// m_SceneSerializer = std::make_unique<SceneSerializer>(m_Scene.get());
	//
	// auto erato = m_Scene->CreateEntity("erato");
	// erato.AddComponent<StaticMeshComponent>("Content/erato.obj").StaticMesh->SetTexture("Content/erato.png");
	// erato.GetComponent<TransformComponent>().Translation = {0.0, 0.1, 0.0};
	// erato.GetComponent<TransformComponent>().Rotation = {0.0, 49.0, 0.0};
	// erato.GetComponent<TransformComponent>().Scale = { 0.1, 0.1, 0.1 };
	//
	// auto floor = m_Scene->CreateEntity("floor");
	// floor.AddComponent<StaticMeshComponent>("Content/floor.obj").StaticMesh->SetTexture("Content/white.png");
	//
	// auto light1 = m_Scene->CreatePointLight();
	// light1.GetComponent<TransformComponent>().Translation = {3.8, 2.8, 0.0};
	//

	//
	// m_SceneSerializer->Serialize("Content/main.scene");
}

void Editor::Update()
{
	if (m_LoadSceneQueued)
	{
		LoadScene();
		m_LoadSceneQueued = false;
	}
	m_Window->PollEvents();
	OnImGuiUpdate();
	m_Scene->Step();
	m_EditorCamera->OnUpdate(Timer::GetDeltaTime());
}

void Editor::Render() const
{
	m_Renderer->Render(*m_EditorCamera, *m_Scene);
}

void Editor::OnImGuiUpdate()
{
	ImGui_ImplGlfw_NewFrame();
	ImGui_ImplVulkan_NewFrame();

	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::Begin("Config");
	ImGui::End();

	ImGui::Begin("Stats");
	ImGui::Text("FPS: %d", (int)(1.0 / (double)Timer::GetDeltaTime()));
	ImGui::Text("Frametime: %.3f ms", (float)Timer::GetDeltaTimeInMillliseconds());
	ImGui::End();

	ImGui::Begin("Content Browser");
	if (ImGui::SmallButton("Import"))
	{
		FileHelper::OpenFile();
	}
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport");
	if (ImGui::SmallButton("Save"))
	{
		SaveScene();
	}
	ImGui::SameLine();
	if (ImGui::SmallButton("Load"))
	{
		m_LoadSceneQueued = true;
	}
	const ImVec2 viewportExtent = ImGui::GetContentRegionAvail();
	m_ViewportExtent.setWidth((uint32_t)viewportExtent.x)
		.setHeight((uint32_t)viewportExtent.y);
	m_EditorCamera->OnResize(m_ViewportExtent.width, m_ViewportExtent.height);
	m_Renderer->OnResize(m_ViewportExtent);
	ImGui::Image(m_Renderer->GetViewportTextureID(), {
					 (float)m_ViewportExtent.width,
					 (float)m_ViewportExtent.height
		});
	ImGui::End();
	ImGui::PopStyleVar();

	m_Outliner->OnImGuiUpdate();

	ImGui::Render();
	ImGui::EndFrame();

	// ImGui::UpdatePlatformWindows();
	// ImGui::RenderPlatformWindowsDefault();
}

void Editor::LoadScene()
{
	std::string filePath = FileHelper::OpenFile("Scene File (*.scene)\0*.scene\0");
	if (!filePath.empty())
	{
		std::unique_ptr<Scene> newScene = std::make_unique<Scene>();
		SceneSerializer serializer(newScene.get());
		if (serializer.Deserialize(filePath))
		{
			m_Scene = std::move(newScene);
			m_ScenePath = filePath;
		}
	}
}

void Editor::SaveScene()
{
	SceneSerializer serializer(m_Scene.get());
	if (!m_ScenePath.empty())
	{
		serializer.Serialize(m_ScenePath);
	}
	else
	{
		std::string filePath = FileHelper::SaveFile("Scene File (*.scene)\0*.scene\0");
		if (!filePath.empty())
		{
			serializer.Serialize(filePath);
			m_ScenePath = filePath;
		}
	}
}
