#include "Mesh.h"

#include <glm/vec3.hpp>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
};

Mesh::Mesh(const std::string& meshPath)
{
    std::vector<Vertex> vertices = {
        {{0.0, 0.5, 0.0}, {1.0, 0.0, 0.0}},
        {{0.5, -0.5, 0.0}, {0.0, 1.0, 0.0}},
        {{-0.5, -0.5, 0.0}, {0.0, 0.0, 1.0}}
    };

    m_VertexBuffer = std::make_unique<Buffer>(vertices.data(), vertices.size() * sizeof(Vertex));
    m_VertexBuffer->SetLayout({
        {ShaderDataType::Float3, "positions"},
        {ShaderDataType::Float3, "colors"}
    });
}
