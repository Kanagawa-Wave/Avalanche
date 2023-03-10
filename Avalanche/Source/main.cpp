#include "Engine/Engine.h"

int main()
{
    const std::unique_ptr<Engine> engine = std::make_unique<Engine>();

    engine->Init();
    engine->Run();
    engine->Destroy();
}