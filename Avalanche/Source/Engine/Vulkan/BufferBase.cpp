#include "BufferBase.h"

#include "Context.h"

BufferBase::BufferBase(vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage, const void* bufferData, size_t size)
    : BufferBase(usage, memoryUsage, size)
{
    Upload(bufferData, size);
}

BufferBase::BufferBase(vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage, size_t size)
    : m_Size(size)
{
    const auto& allocator = Context::Instance().GetAllocator();

    vk::BufferCreateInfo bufferInfo;
    bufferInfo.setSize(size)
              .setUsage(usage);

    VmaAllocationCreateInfo vmaInfo{};
    vmaInfo.usage = memoryUsage;

    if (vmaCreateBuffer(allocator, (VkBufferCreateInfo*)&bufferInfo, &vmaInfo, (VkBuffer*)&m_Buffer, &m_Allocation,
                        nullptr) != VK_SUCCESS)
    {
        ASSERT(0, "[vma] Failed to create buffer")
    }
}

BufferBase::~BufferBase()
{
    const auto& allocator = Context::Instance().GetAllocator();
    vmaDestroyBuffer(allocator, m_Buffer, m_Allocation);
}

void BufferBase::Upload(const void* bufferData, size_t size) const
{
    const auto& allocator = Context::Instance().GetAllocator();
    void* data;

    vmaMapMemory(allocator, m_Allocation, &data);
    memcpy(data, bufferData, size);
    vmaUnmapMemory(allocator, m_Allocation);
}

void BufferBase::Bind(vk::CommandBuffer commandBuffer)
{
    LOG_W("Attempt to bind a non-bindable buffer to the commandbuffer!")
}
