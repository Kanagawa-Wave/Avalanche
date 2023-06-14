#include "Buffers.h"

#include "Renderer/Context.h"

VertexBuffer::VertexBuffer(const void* vertices, size_t size)
    : BufferBase(vk::BufferUsageFlagBits::eVertexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, vertices, size)
{
}

void VertexBuffer::Bind(vk::CommandBuffer commandBuffer)
{
    commandBuffer.bindVertexBuffers(0, m_Buffer, {0});
}

IndexBuffer::IndexBuffer(const uint32_t* indices, size_t count)
    : BufferBase(vk::BufferUsageFlagBits::eIndexBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, indices, count * sizeof(uint32_t)),
      m_Count((uint32_t)count)
{
}

void IndexBuffer::Bind(vk::CommandBuffer commandBuffer)
{
    commandBuffer.bindIndexBuffer(m_Buffer, 0, vk::IndexType::eUint32);
}

Buffer::Buffer(vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage, size_t size)
    : BufferBase(usage, memoryUsage, size)
{
}

void Buffer::SetData(const void* src) const
{
    const auto& allocator = Context::Instance().GetAllocator();
    
    void* dst;
    vmaMapMemory(allocator, m_Allocation, &dst);
    memcpy(dst, src, m_Size);
    vmaUnmapMemory(allocator, m_Allocation);
}


