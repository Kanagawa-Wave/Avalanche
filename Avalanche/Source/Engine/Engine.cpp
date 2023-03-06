#include "Engine.h"

#include "Log/Log.h"

void Engine::Init()
{
    Log::Init();

    m_Window = std::make_unique<Window>(800, 600, "Avalanche");

    Context::Init(m_Window->GetGLFWWindow());
    Context::GetInstance().CreateSwapchain(m_Window->GetGLFWWindow());
}

void Engine::Destroy()
{
    Context::GetInstance().Destroy();
}

void Engine::Draw()
{

}

void Engine::Run()
{
    while (m_Window->Running())
    {
        m_Window->PollEvents();
        Draw();
    }
}
