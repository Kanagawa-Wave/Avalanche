#include "Buffer.h"

#include "Context.h"

Buffer::Buffer(void* bufferData, uint32_t size)
{
    const auto& allocator = Context::Instance().GetAllocator();

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    VmaAllocationCreateInfo vmaInfo{};
    vmaInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    if (vmaCreateBuffer(allocator, &bufferInfo, &vmaInfo, (VkBuffer*)&m_Buffer, &m_Allocation,
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
