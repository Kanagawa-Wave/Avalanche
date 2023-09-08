#pragma once
#include "Mesh.h"

class Model
{
public:
    Model() = default;
    Model(const std::string& path);
    
    std::vector<Mesh> m_Meshes;
private:
    void LoadModelAssimp(const std::string& path);
    void ProcessNode(const aiScene* scene, const aiNode* node);
    
};
