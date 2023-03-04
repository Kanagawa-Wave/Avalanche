#include "Engine.h"

#include "Log/Log.h"

void Engine::Init()
{
    Log::Init();

    m_Window = std::make_unique<Window>(800, 600, "Avalanche");
    m_Context = std::make_unique<Context>(m_Window->GetGLFWWindow());

    m_Context->CreateSwapchain(m_Window->GetWidth(), m_Window->GetHeight());
    m_Context->CreatePipeline("Shaders/Triangle");
}

void Engine::Destroy()
{
}

void Engine::Draw()
{
    m_Context->Begin({{0.2f, 0.2f, 0.8f, 1.0f}});

    m_Context->Draw(3);
    
    m_Context->End();
    m_Context->Submit();
}

void Engine::Run()
{
    while (m_Window->Running())
    {
        m_Window->PollEvents();
        Draw();
    }
}
