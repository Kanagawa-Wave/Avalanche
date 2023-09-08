#include "Model.h"

Model::Model(const std::string& path)
{
    // TODO: move tiny_obj code here
    LoadModelAssimp(path);
}

void Model::LoadModelAssimp(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        ASSERT(0, importer.GetErrorString())
    }

    ProcessNode(scene, scene->mRootNode);
}

void Model::ProcessNode(const aiScene* scene, const aiNode* node)
{
    // Process all meshes in this node
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        m_Meshes.emplace_back(Mesh(scene->mMeshes[node->mMeshes[i]]));
    }

    // Process all child nodes of this node
    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(scene, node->mChildren[i]);
    }
}
