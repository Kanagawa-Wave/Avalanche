#include "DescriptorSet.h"

#include "Renderer/Context.h"

DescriptorSet::DescriptorSet(vk::DescriptorPool pool, vk::DescriptorSetLayout layout)
{
	const auto& device = Context::Instance().GetDevice();

	m_DescriptorSetLayout = layout;

	vk::DescriptorSetAllocateInfo allocateInfo;
	allocateInfo.setDescriptorPool(pool)
		.setSetLayouts(m_DescriptorSetLayout)
		.setDescriptorSetCount(1);

	m_DescriptorSet = device.allocateDescriptorSets(allocateInfo).front();
}

DescriptorSet::~DescriptorSet() = default;

void DescriptorSet::AttachUniformBuffer(const Buffer* buffer, uint32_t binding) const
{
	const auto& device = Context::Instance().GetDevice();

	vk::DescriptorBufferInfo bufferInfo;
	bufferInfo.setBuffer(buffer->GetBuffer())
		.setOffset(0)
		.setRange(buffer->GetSize());

	vk::WriteDescriptorSet writeInfo;
	writeInfo.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eUniformBuffer)
		.setBufferInfo(bufferInfo)
		.setDstBinding(binding)
		.setDstSet(m_DescriptorSet);

	device.updateDescriptorSets(writeInfo, nullptr);
}

void DescriptorSet::AttachTexture(const Texture* texture, uint32_t binding) const
{
	const auto& device = Context::Instance().GetDevice();

	vk::DescriptorImageInfo imageInfo;
	imageInfo.setSampler(texture->GetSampler())
		.setImageView(texture->GetView())
		.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

	vk::WriteDescriptorSet writeInfo;
	writeInfo.setDescriptorCount(1)
		.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
		.setImageInfo(imageInfo)
		.setDstBinding(binding)
		.setDstSet(m_DescriptorSet);

	device.updateDescriptorSets(writeInfo, nullptr);
}
