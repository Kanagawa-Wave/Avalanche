#pragma once

#include "Application.h"

extern Application* CreateApplication();

inline int main()
{
    Application* app = Application::Instance();
    app->Run();
    delete app;
}