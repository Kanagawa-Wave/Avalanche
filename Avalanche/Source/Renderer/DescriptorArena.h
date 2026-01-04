#pragma once
#include <vulkan/vulkan.hpp>

class DescriptorArena {
public:
    DescriptorArena(uint32_t framesInFlight,
                    uint32_t setsPerPool = 4096);
    ~DescriptorArena();

    void Flip();                                      // call once per frame
    vk::DescriptorSet Allocate(vk::DescriptorSetLayout layout);

private:
    struct Pool {
        vk::DescriptorPool pool;
        uint32_t           used = 0;
        uint32_t           cap  = 0;
    };
    std::vector<Pool>          m_Pools;              // size == frames-in-flight
    uint32_t                   m_Current = 0;
    uint32_t                   m_SetsPerPool;
};

