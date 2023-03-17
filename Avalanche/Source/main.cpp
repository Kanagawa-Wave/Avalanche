#include "Engine/Application.h"

int main()
{
    Application::InitInstance();
    Application& app = Application::Instance(); 

    app.Init();
    app.Run();
    app.Destroy();
}