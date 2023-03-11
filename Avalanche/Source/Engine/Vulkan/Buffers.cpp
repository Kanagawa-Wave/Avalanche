#include "Buffers.h"

#include "Context.h"

Buffer::Buffer(vk::BufferUsageFlags usage, const void* bufferData, uint32_t size)
{
    const auto& allocator = Context::Instance().GetAllocator();

    vk::BufferCreateInfo bufferInfo;
    bufferInfo.setSize(size)
              .setUsage(usage);

    VmaAllocationCreateInfo vmaInfo{};
    vmaInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    if (vmaCreateBuffer(allocator, (VkBufferCreateInfo*)&bufferInfo, &vmaInfo, (VkBuffer*)&m_Buffer, &m_Allocation,
                        nullptr) != VK_SUCCESS)
    {
        ASSERT(0, "[vma] Failed to create buffer")
    }

    void* data;
    vmaMapMemory(allocator, m_Allocation, &data);
    memcpy(data, bufferData, size);
    vmaUnmapMemory(allocator, m_Allocation);
}

Buffer::~Buffer()
{
    const auto& allocator = Context::Instance().GetAllocator();
    vmaDestroyBuffer(allocator, m_Buffer, m_Allocation);
}

VertexBuffer::VertexBuffer(const void* vertices, uint32_t size)
    : Buffer(vk::BufferUsageFlagBits::eVertexBuffer, vertices, size)
{
}

IndexBuffer::IndexBuffer(const uint32_t* indices, uint32_t count)
    : Buffer(vk::BufferUsageFlagBits::eIndexBuffer, indices, count * sizeof(uint32_t)), m_Count(count)
{
}
