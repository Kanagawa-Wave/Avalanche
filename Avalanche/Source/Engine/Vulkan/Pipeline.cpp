#include "Pipeline.h"

#include "Initializer.h"
#include "PipelineBuilder.h"
#include "Engine/Log/Log.h"

Pipeline::Pipeline(vk::Device device, vk::RenderPass renderPass, vk::Extent2D extent, const std::string& vsh,
                   const std::string& fsh)
{
    vk::ShaderModule vsm, fsm;
    LoadShaderModule(device, vsh, ShaderType::eVertex, vsm);
    LoadShaderModule(device, fsh, ShaderType::eFragment, fsm);

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo = Initializer::PipelineLayout();
    m_PipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

    PipelineBuilder pipelineBuilder;
    pipelineBuilder.ShaderStages.push_back(
        Initializer::PipelineShaderStage(vk::ShaderStageFlagBits::eVertex, vsm));
    pipelineBuilder.ShaderStages.push_back(
        Initializer::PipelineShaderStage(vk::ShaderStageFlagBits::eFragment, fsm));
    pipelineBuilder.VertexInputInfo = Initializer::PipelineVertexInput();
    pipelineBuilder.InputAssembly = Initializer::PipelineInputAssembly(vk::PrimitiveTopology::eTriangleList);
    pipelineBuilder.Viewport.setX(0.f)
                   .setY(0.f)
                   .setWidth((float)extent.width)
                   .setHeight((float)extent.height)
                   .setMinDepth(0.f)
                   .setMaxDepth(1.f);
    pipelineBuilder.Scissor.setExtent(extent)
                   .setOffset({0, 0});
    pipelineBuilder.Rasterizer = Initializer::PipelineRasterization(vk::PolygonMode::eFill);
    pipelineBuilder.Multisampling = Initializer::PipelineMultisample();
    pipelineBuilder.ColorBlendAttachment = Initializer::PipelineColorBlendAttachment();
    pipelineBuilder.PipelineLayout = m_PipelineLayout;

    m_Pipeline = pipelineBuilder.Build(device, renderPass);
}

void Pipeline::LoadShaderModule(vk::Device device, std::string path, ShaderType type, vk::ShaderModule& shaderModule)
{
    switch (type)
    {
    case ShaderType::eVertex:
        path.append(".vert.spv");
        break;
    case ShaderType::eFragment:
        path.append(".frag.spv");
        break;
    case ShaderType::eCompute:
        path.append(".comp.spv");
        break;
    }
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    ASSERT(file.is_open(), "Failed to open shader file at {}", path)

    const long long fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    std::vector<uint32_t> code(buffer.begin(), buffer.end());

    vk::ShaderModuleCreateInfo shaderInfo;
    // shaderInfo.setCode(code);
    shaderInfo.setPCode(reinterpret_cast<const uint32_t*>(buffer.data())).setCodeSize(buffer.size());

    shaderModule = device.createShaderModule(shaderInfo);
}
