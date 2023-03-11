#pragma once

#include "Engine/Vulkan/Buffer.h"

class Mesh
{
public:
    Mesh(const std::string& meshPath);

    Buffer& GetVertexBuffer() const { return *m_VertexBuffer; }

private:
    std::unique_ptr<Buffer> m_VertexBuffer;
};
