#include "Engine/Engine.h"

int main()
{
    Engine* engine = new Engine();

    engine->Init();
    engine->Run();
    engine->Destroy();

    delete engine;
}