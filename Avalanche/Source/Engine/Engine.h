#pragma once

#include "Window/Window.h"
#include "Vulkan/Context.h"

class Engine
{
public:
    void Init();
    void Destroy();

    void Draw();
    void Run();

private:
    std::unique_ptr<Window> m_Window;
};
