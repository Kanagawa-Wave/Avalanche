﻿#pragma once

#include <Avalanche.h>
#include <EntryPoint.h>

class Editor : public Application
{
public:
    Editor();
    virtual void Update() override;
    virtual void Render() const override;

private:

    void OnImGuiUpdate();

    void LoadScene();
    void SaveScene();
    
    vk::Extent2D m_ViewportExtent;
    
    std::unique_ptr<Camera> m_EditorCamera;
    
    std::unique_ptr<Scene> m_Scene;
    std::string m_ScenePath;

    bool m_LoadSceneQueued = false;
    
    std::unique_ptr<Outliner> m_Outliner;
};