#include "DescriptorArena.h"

#include "Context.h"

static std::vector<vk::DescriptorPoolSize> DefaultSizes(uint32_t c) {
    return {
            {vk::DescriptorType::eUniformBuffer,        c},
            {vk::DescriptorType::eUniformBufferDynamic, c},
            {vk::DescriptorType::eStorageBuffer,        c},
            {vk::DescriptorType::eCombinedImageSampler, c},
            {vk::DescriptorType::eSampler,              c}
    };
}

DescriptorArena::DescriptorArena(uint32_t framesInFlight,
                                 uint32_t setsPerPool)
    : m_Pools(framesInFlight), m_SetsPerPool(setsPerPool)
{
    auto device = Context::Instance().GetDevice();
    
    for (auto& p : m_Pools) {
        vk::DescriptorPoolCreateInfo info{};
        std::vector<vk::DescriptorPoolSize> poolSizes = DefaultSizes(m_SetsPerPool);
        info.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setMaxSets(m_SetsPerPool)
            .setPoolSizes(poolSizes);
        p.pool = device.createDescriptorPool(info);
        p.cap  = m_SetsPerPool;
    }
}

void DescriptorArena::Flip() {
    m_Current = (m_Current + 1) % m_Pools.size();
    m_Pools[m_Current].used = 0;         // bump-pointer reset
}

vk::DescriptorSet DescriptorArena::Allocate(vk::DescriptorSetLayout layout) {
    auto device = Context::Instance().GetDevice();
    
    Pool& P = m_Pools[m_Current];
    if (P.used == P.cap) {               // out of space → grow once
        vk::DescriptorPoolCreateInfo info{};
        std::vector<vk::DescriptorPoolSize> poolSizes = DefaultSizes(m_SetsPerPool);
        info.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setMaxSets(m_SetsPerPool)
            .setPoolSizes(poolSizes);
        P.pool = device.createDescriptorPool(info);
        P.used = 0;
        P.cap  = m_SetsPerPool;
    }
    vk::DescriptorSetAllocateInfo ai{P.pool, 1, &layout};
    ++P.used;
    return std::move(device.allocateDescriptorSets(ai).front());
}

DescriptorArena::~DescriptorArena()
{
    auto device = Context::Instance().GetDevice();

    device.waitIdle();
    for (auto& p : m_Pools)
        if (p.pool) device.destroyDescriptorPool(p.pool);
}
