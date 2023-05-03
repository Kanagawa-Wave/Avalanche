#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

class BufferBase
{
public:
    BufferBase(vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage, const void* bufferData, size_t size);
    BufferBase(vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage, size_t size);
    virtual ~BufferBase();

    void Upload(const void* bufferData, size_t size) const;

    virtual void Bind(vk::CommandBuffer commandBuffer);
    
    const vk::Buffer& GetBuffer() const { return m_Buffer; }
    const VmaAllocation& GetAllocation() const { return m_Allocation; }
    const size_t GetSize() const { return m_Size; }

protected:
    vk::Buffer m_Buffer;
    VmaAllocation m_Allocation = VK_NULL_HANDLE;

    size_t m_Size = 0;
};