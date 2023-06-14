#pragma once

#include "Application.h"

extern Application* CreateApplication();

inline int main()
{
    const Application* app = Application::Instance();
    app->Run();
    delete app;
}