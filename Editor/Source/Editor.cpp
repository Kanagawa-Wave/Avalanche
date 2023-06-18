#include "Editor.h"

#include <imgui.h>




Application* CreateApplication()
{
    return new Editor();
}

Editor::Editor()
{
    m_ViewportExtent = m_Window->GetExtent();
    
    m_Renderer = std::make_unique<Renderer>(m_Window.get(), true);
    m_EditorCamera = std::make_unique<Camera>(m_ViewportExtent.width, m_ViewportExtent.height, 30.0f, 0.1f, 1000.0f);
    m_Renderer->SetCameraPtr(m_EditorCamera.get());
    m_Renderer->SetExtentPtr(&m_ViewportExtent);
    
    m_TestMesh = std::make_unique<Mesh>("Content/bunny.obj");
    m_TestMesh->AddTexture("Content/bunny.png");
    m_Renderer->AppendToDrawList(m_TestMesh.get());
}

void Editor::Update()
{
    m_Window->PollEvents();
    OnImGuiUpdate();
    m_EditorCamera->OnUpdate(Timer::Elapsed());
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
    
    ImGui::Begin("Details");
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");
    const ImVec2 viewportExtent = ImGui::GetContentRegionAvail();
    m_ViewportExtent.setWidth((uint32_t)viewportExtent.x)
                    .setHeight((uint32_t)viewportExtent.y);
    m_EditorCamera->Resize(m_ViewportExtent.width, m_ViewportExtent.height);
    m_Renderer->ResizeViewport(m_ViewportExtent);
    ImGui::Image(m_Renderer->GetViewportTextureID(), {
                     (float)m_ViewportExtent.width,
                     (float)m_ViewportExtent.height
                 });
    ImGui::End();
    ImGui::PopStyleVar();
    
    ImGui::Render();
    ImGui::EndFrame();
}
