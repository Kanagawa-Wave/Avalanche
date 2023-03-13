#pragma once

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

class Buffer
{
public:
    Buffer(vk::BufferUsageFlags usage, const void* bufferData, size_t size);
    ~Buffer();

    const vk::Buffer& GetBuffer() const { return m_Buffer; }

private:
    vk::Buffer m_Buffer;
    VmaAllocation m_Allocation = VK_NULL_HANDLE;
};