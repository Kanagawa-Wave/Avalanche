#pragma once

#include <Avalanche.h>

class Outliner
{
public:
    Outliner() = default;
    Outliner(Scene* scene);

    void SetContext(Scene* scene);
    void OnImGuiUpdate();

private:
    void DrawEntityNode(Entity entity);
    void DrawComponents(Entity entity);
    
    Scene* m_Context = nullptr;
    Entity m_SelectedEntity;
};