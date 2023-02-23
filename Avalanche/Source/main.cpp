#include <VkBootstrap.h>

#include <vulkan/vulkan.hpp>

int main()
{
    vkb::InstanceBuilder builder;

    //make the vulkan instance, with basic debug features
    auto inst_ret = builder.set_app_name("Example Vulkan Application")
        .request_validation_layers(true)
        .use_default_debug_messenger()
        .require_api_version(1, 1, 0)
        .build();

    vkb::Instance vkb_inst = inst_ret.value();

    //grab the instance 
    vk::Instance instance = vkb_inst.instance;
    vk::DebugUtilsMessengerEXT debugMessager = vkb_inst.debug_messenger;
    
}