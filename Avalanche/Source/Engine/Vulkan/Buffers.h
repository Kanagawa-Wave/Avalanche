#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

#include "Engine/Log/Log.h"
#include "BufferBase.h"

enum class ShaderDataType : uint8_t
{
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Bool
};

static uint32_t GetShaderDataTypeSize(ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float: return 4;
    case ShaderDataType::Float2: return 4 * 2;
    case ShaderDataType::Float3: return 4 * 3;
    case ShaderDataType::Float4: return 4 * 4;
    case ShaderDataType::Mat3: return 4 * 3 * 3;
    case ShaderDataType::Mat4: return 4 * 4 * 4;
    case ShaderDataType::Int: return 4;
    case ShaderDataType::Int2: return 4 * 2;
    case ShaderDataType::Int3: return 4 * 3;
    case ShaderDataType::Int4: return 4 * 4;
    case ShaderDataType::Bool: return 1;
    case ShaderDataType::None:
        LOG_E("ShaderDataType = None!")
        return 0;
    default:
        LOG_E("Invalid ShaderDataType!")
        return 0;
    }
}

struct BufferElement
{
    std::string Name;
    ShaderDataType Type;
    vk::Format format;
    uint32_t Size;
    uint32_t Offset;
    bool Normalized;

    uint32_t GetComponentCount() const
    {
        switch (Type)
        {
        case ShaderDataType::Float: return 1;
        case ShaderDataType::Float2: return 2;
        case ShaderDataType::Float3: return 3;
        case ShaderDataType::Float4: return 4;
        case ShaderDataType::Mat3: return 3 * 3;
        case ShaderDataType::Mat4: return 4 * 4;
        case ShaderDataType::Int: return 1;
        case ShaderDataType::Int2: return 2;
        case ShaderDataType::Int3: return 3;
        case ShaderDataType::Int4: return 4;
        case ShaderDataType::Bool: return 1;
        case ShaderDataType::None: LOG_E("Invalid ShaderDataType!")
        }

        return 0;
    }

    BufferElement() = default;

    BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
        : Name(name), Type(type), Size(GetShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
    {
        switch (type)
        {
        case ShaderDataType::Float: format = vk::Format::eR32Sfloat;
            break;
        case ShaderDataType::Float2: format = vk::Format::eR32G32Sfloat;
            break;
        case ShaderDataType::Float3: format = vk::Format::eR32G32B32Sfloat;
            break;
        case ShaderDataType::Float4: format = vk::Format::eR32G32B32A32Sfloat;
            break;
        case ShaderDataType::Mat3: format = vk::Format::eUndefined;
            break;
        case ShaderDataType::Mat4: format = vk::Format::eUndefined;
            break;
        case ShaderDataType::Int: format = vk::Format::eR32Sint;
            break;
        case ShaderDataType::Int2: format = vk::Format::eR32G32Sint;
            break;
        case ShaderDataType::Int3: format = vk::Format::eR32G32B32Sint;
            break;
        case ShaderDataType::Int4: format = vk::Format::eR32G32B32A32Sint;
            break;
        case ShaderDataType::Bool: format = vk::Format::eR8Sint;
            break;
        case ShaderDataType::None: LOG_E("Invalid ShaderDataType!")
            break;
        }
    }
};

struct VertexInputInfo
{
    std::vector<vk::VertexInputAttributeDescription> AttributeDescriptions;
    vk::VertexInputBindingDescription BindingDescription;
};

class BufferLayout
{
public:
    BufferLayout() = default;

    BufferLayout(const std::initializer_list<BufferElement>& elements) : m_Elements(elements)
    {
        uint32_t offset = 0, location = 0;
        m_Stride = 0;
        for (auto& element : m_Elements)
        {
            element.Offset = offset;

            vk::VertexInputAttributeDescription description;
            description.setBinding(0)
                       .setLocation(location)
                       .setFormat(element.format)
                       .setOffset(element.Offset);
            m_VertexInput.AttributeDescriptions.push_back(description);

            offset += element.Size;
            m_Stride += element.Size;
            location++;
        }

        m_VertexInput.BindingDescription.setBinding(0)
                            .setStride(m_Stride)
                            .setInputRate(vk::VertexInputRate::eVertex);
    }

    inline uint32_t GetStride() const { return m_Stride; }
    inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
    inline const VertexInputInfo& GetVertexInputInfo() const { return m_VertexInput; }

    std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
    std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
    std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
    std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

private:
    std::vector<BufferElement> m_Elements;
    VertexInputInfo m_VertexInput;
    uint32_t m_Stride = 0;
};

class VertexBuffer final : public Buffer
{
public:
    VertexBuffer(const void* vertices, uint32_t size);
    ~VertexBuffer() = default;

    const BufferLayout& GetLayout() const { return m_Layout; }
    void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

private:
    BufferLayout m_Layout;
};

class IndexBuffer final : public Buffer
{
public:
    IndexBuffer(const uint32_t* indices, uint32_t count);
    ~IndexBuffer() = default;

    uint32_t GetCount() const { return m_Count; }
    
private:
    uint32_t m_Count = 0;
};