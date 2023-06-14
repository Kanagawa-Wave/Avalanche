#include "Mesh.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <tiny_obj_loader.h>

template <typename T, typename... Rest>
void hashCombine(std::size_t& seed, const T& v, const Rest&... rest)
{
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
};

namespace std
{
    template <>
    struct hash<Vertex>
    {
        size_t operator()(Vertex const& vertex) const noexcept
        {
            size_t seed = 0;
            hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
}

Mesh::Mesh(const std::string& meshPath)
{
    LoadObjFromFile(meshPath);
    
    m_VertexBuffer = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex));
    m_VertexBuffer->SetLayout(Vertex::Layout());

    m_IndexBuffer = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());
}

void Mesh::Bind(vk::CommandBuffer commandBuffer) const
{
    m_VertexBuffer->Bind(commandBuffer);
    m_IndexBuffer->Bind(commandBuffer);
}

void Mesh::Draw(vk::CommandBuffer commandBuffer) const
{
    commandBuffer.drawIndexed(m_IndexBuffer->GetCount(), 1, 0, 0, 0);
}

void Mesh::AddTexture(const std::string& path)
{
    m_Texture = std::make_unique<Texture>(path);
}

void Mesh::LoadObjFromFile(const std::string& path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, error;

    if (!tinyobj::LoadObj(
        &attrib,
        &shapes,
        &materials,
        &warn,
        &error,
        path.c_str()))
    {
        LOG_E("{0}", warn + error)
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Vertex vertex{};
            if (index.vertex_index >= 0)
            {
                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2],
                };
                vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0],
                    attrib.colors[3 * index.vertex_index + 1],
                    attrib.colors[3 * index.vertex_index + 2],
                };
            }
            if (index.normal_index >= 0)
            {
                vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2],
                };
            }
            if (index.texcoord_index >= 0)
            {
                vertex.uv = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1 - attrib.texcoords[2 * index.texcoord_index + 1],
                };
            }


            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
                m_Vertices.push_back(vertex);
            }
            m_Indices.push_back(uniqueVertices[vertex]);
        }
    }
}
