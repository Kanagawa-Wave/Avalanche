#include "Texture.h"

#include <stb_image.h>

#include "Context.h"
#include "ImmediateContext.h"
#include "Core/Log.h"
#include "ImGui/imgui_impl_vulkan.h"
#include "Vulkan/Buffers.h"

Texture::Texture(vk::Format format, vk::Extent2D extent, vk::ImageUsageFlags usage)
{
    const auto& device = Context::Instance().GetDevice();
    m_Image = std::make_unique<Image>(format, extent, usage);

    vk::SamplerCreateInfo samplerInfo;
    samplerInfo.setMagFilter(vk::Filter::eLinear)
               .setMinFilter(vk::Filter::eLinear)
               .setAddressModeU(vk::SamplerAddressMode::eRepeat)
               .setAddressModeV(vk::SamplerAddressMode::eRepeat)
               .setAddressModeW(vk::SamplerAddressMode::eRepeat);

    m_Sampler = device.createSampler(samplerInfo);
}

Texture::Texture(const std::string& path, vk::Format format)
{
    // Read pixel data using stbi_image
    const auto& allocator = Context::Instance().GetAllocator();
    const auto& device = Context::Instance().GetDevice();

    stbi_uc* pixelData = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, STBI_rgb_alpha);

    ASSERT(pixelData, "Failed to load texture")

    vk::DeviceSize imageSize = (vk::DeviceSize)m_Width * m_Height * m_Channels;

    // Upload pixel data to the staging buffer
    std::unique_ptr<Buffer> stagingBuffer = std::make_unique<Buffer>(vk::BufferUsageFlagBits::eTransferSrc,
                                                                     VMA_MEMORY_USAGE_CPU_ONLY, imageSize);

    void* data;
    vmaMapMemory(allocator, stagingBuffer->GetAllocation(), &data);
    memcpy(data, pixelData, imageSize);
    vmaUnmapMemory(allocator, stagingBuffer->GetAllocation());

    stbi_image_free(pixelData);

    m_Image = std::make_unique<Image>(format, vk::Extent2D((uint32_t)m_Width, (uint32_t)m_Height),
                                      vk::ImageUsageFlagBits::eSampled |
                                      vk::ImageUsageFlagBits::eTransferDst);

    ImmediateContext::Submit([&](vk::CommandBuffer commandBuffer)
    {
        vk::ImageSubresourceRange range;
        range.setAspectMask(vk::ImageAspectFlagBits::eColor)
             .setLayerCount(1)
             .setLevelCount(1)
             .setBaseArrayLayer(0)
             .setBaseMipLevel(0);

        vk::ImageMemoryBarrier imageBarrierToTransfer;
        imageBarrierToTransfer.setImage(m_Image->GetImage())
                              .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
                              .setOldLayout(vk::ImageLayout::eUndefined)
                              .setSubresourceRange(range)
                              .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
                              .setSrcAccessMask(vk::AccessFlagBits::eNone);

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
                                      vk::DependencyFlagBits::eByRegion, nullptr, nullptr, imageBarrierToTransfer);

        vk::BufferImageCopy copyRegion;
        copyRegion.setImageSubresource({vk::ImageAspectFlagBits::eColor, 0, 0, 1})
                  .setImageExtent({(uint32_t)m_Width, (uint32_t)m_Height, 1});

        commandBuffer.copyBufferToImage(stagingBuffer->GetBuffer(), m_Image->GetImage(),
                                        vk::ImageLayout::eTransferDstOptimal, copyRegion);

        vk::ImageMemoryBarrier imageBarrierToShaderReadable;
        imageBarrierToShaderReadable.setImage(m_Image->GetImage())
                                    .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                                    .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                                    .setSubresourceRange(range)
                                    .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
                                    .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);

        commandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
                                      vk::DependencyFlagBits::eByRegion, nullptr, nullptr,
                                      imageBarrierToShaderReadable);
    });

    vk::SamplerCreateInfo samplerInfo;
    samplerInfo.setMagFilter(vk::Filter::eLinear)
               .setMinFilter(vk::Filter::eLinear)
               .setAddressModeU(vk::SamplerAddressMode::eRepeat)
               .setAddressModeV(vk::SamplerAddressMode::eRepeat)
               .setAddressModeW(vk::SamplerAddressMode::eRepeat);

    m_Sampler = device.createSampler(samplerInfo);
}

Texture::~Texture()
{
    const auto& device = Context::Instance().GetDevice();

    device.waitIdle();
    device.destroySampler(m_Sampler);
}

void Texture::RegisterForImGui()
{
    m_DescriptorSet = ImGui_ImplVulkan_AddTexture(m_Sampler, m_Image->GetView(),
                                                  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
