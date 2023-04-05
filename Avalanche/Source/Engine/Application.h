#pragma once

#include "Scene/Components/Mesh.h"
#include "Window/Window.h"
#include "Vulkan/Context.h"
#include "Vulkan/Renderer.h"

class Application
{
public:
    static Application& Instance() { return *s_Instance; }
    ~Application() = default;

    static void InitInstance();
    void Init();
    void Destroy();

    void Draw() const;
    void Run();

    Window& GetWindow() const { return *m_Window; }

private:
    Application() = default;

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Renderer> m_Renderer;

    // TODO: remove
    std::unique_ptr<Mesh> m_Triangle;

    static std::unique_ptr<Application> s_Instance;
};
