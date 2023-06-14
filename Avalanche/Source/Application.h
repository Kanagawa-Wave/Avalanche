#pragma once

#include "Renderer/Renderer.h"
#include "Window/Window.h"

class Application
{
public:
    Application();
    virtual ~Application();

    virtual void Update() const = 0;
    virtual void Render() const = 0;
    void Run() const;

    Window* GetWindow() const { return m_Window.get(); }
    static Application* Instance() { return s_Application; }

protected:
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Renderer> m_Renderer;

    static Application* s_Application;
};

Application* CreateApplication();