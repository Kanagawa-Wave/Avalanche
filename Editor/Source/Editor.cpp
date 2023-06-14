#include "Editor.h"

Application* CreateApplication()
{
    return new Editor();
}

Editor::Editor()
{
    m_ViewportExtent = m_Window->GetExtent();
    
    m_Renderer = std::make_unique<Renderer>(m_Window.get(), true);
    m_EditorCamera = std::make_unique<Camera>(m_ViewportExtent.width, m_ViewportExtent.height, 30.0f, 0.001f, 100.0f);

    m_TestMesh = std::make_unique<Mesh>("Content/bunny.obj");
    m_TestMesh->AddTexture("Content/bunny.png");
    m_Renderer->AppendToDrawList(m_TestMesh.get());
}

void Editor::Update() const
{
    m_Window->PollEvents();
    m_Renderer->OnUpdate(Timer::Elapsed());
    m_EditorCamera->OnUpdate(Timer::Elapsed());
}

void Editor::Render() const
{
    m_Renderer->OnRender();
}
