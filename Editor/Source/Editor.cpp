#include "Editor.h"

#include <imgui.h>

#include "Panels/Outliner.h"

Application* CreateApplication()
{
    return new Editor();
}

Editor::Editor()
{
    m_ViewportExtent = m_Window->GetExtent();
    m_EditorCamera = Camera(m_ViewportExtent.width, m_ViewportExtent.height, 30.0f, 0.1f, 1000.0f);

    m_Renderer = std::make_unique<Renderer>(m_Window.get(), m_EditorCamera, m_ViewportExtent);

    m_Scene = std::make_unique<Scene>();
    
    auto bunny = m_Scene->CreateEntity("bunny");
    bunny.AddComponent<MeshComponent>("Content/bunny.obj")
         .SetTexture("Content/bunny.png");
    bunny.GetComponent<TransformComponent>().Scale = {10.f, 10.f, 10.f};

    m_Renderer->SubmitScene(m_Scene.get());
    
    m_Outliner = std::make_unique<Outliner>(m_Scene.get());
}

void Editor::Update()
{
    m_Window->PollEvents();
    OnImGuiUpdate();
    m_EditorCamera.OnUpdate(Timer::GetDeltaTime());
}

void Editor::Render() const
{
    m_Renderer->OnRender();
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
    const ImVec2 viewportExtent = ImGui::GetContentRegionAvail();
    m_ViewportExtent.setWidth((uint32_t)viewportExtent.x)
                    .setHeight((uint32_t)viewportExtent.y);
    m_EditorCamera.Resize(m_ViewportExtent.width, m_ViewportExtent.height);
    m_Renderer->ResizeViewport(m_ViewportExtent);
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
