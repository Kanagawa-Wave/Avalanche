#pragma once

#include "Scene/Components/Mesh.h"
#include "Window/Window.h"
#include "Vulkan/Context.h"
#include "Vulkan/Renderer.h"

class Engine
{
public:
    void Init();
    void Destroy();

    void Draw();
    void Run();

private:
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Renderer> m_Renderer;

    // TODO: remove
    std::unique_ptr<Mesh> m_Triangle;
};
