#include "Initializers.hpp"
#include <vulkan/vulkan_core.h>

VkImageCreateInfo defaultImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent){
    return VkImageCreateInfo{
        .sType = STYPE(IMAGE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = extent,
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usageFlags
    };
}

VkImageViewCreateInfo defaultImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags){
    return VkImageViewCreateInfo{
        .sType = STYPE(IMAGE_VIEW_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange = {
            .aspectMask = aspectFlags,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };
}

VkPipelineDepthStencilStateCreateInfo defaultPipelineDepthStencilStateCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp){
    VkPipelineDepthStencilStateCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    info.pNext = nullptr;

    info.depthTestEnable = bDepthTest ? VK_TRUE : VK_FALSE;
    info.depthWriteEnable = bDepthWrite ? VK_TRUE : VK_FALSE;
    info.depthCompareOp = bDepthTest ? compareOp : VK_COMPARE_OP_ALWAYS;
    info.depthBoundsTestEnable = VK_FALSE;
    info.minDepthBounds = 0.0f; // Optional
    info.maxDepthBounds = 1.0f; // Optional
    info.stencilTestEnable = VK_FALSE;

    return info;
}

VkCommandPoolCreateInfo defaultCommandPoolCreateInfo(uint32_t queueIdx, VkCommandPoolCreateFlags flags){
    // create info command pool for graphics queue
    return VkCommandPoolCreateInfo{
        .sType = STYPE(COMMAND_POOL_CREATE_INFO),
        .pNext = nullptr,
        .flags = flags,
        .queueFamilyIndex = queueIdx
    };
}

VkCommandBufferAllocateInfo defaultCommandBufferAllocateInfo(VkCommandPool commandPool, uint32_t count){
    return VkCommandBufferAllocateInfo{
        .sType = STYPE(COMMAND_BUFFER_ALLOCATE_INFO),
        .pNext = nullptr,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = count
    };
}

VkSemaphoreCreateInfo defaultSemaphoreCreateInfo() {
    return VkSemaphoreCreateInfo{
        .sType = STYPE(SEMAPHORE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0
    };
}

VkFenceCreateInfo defaultFenceCreateInfo(VkFenceCreateFlags flags){
    return VkFenceCreateInfo{
        .sType = STYPE(FENCE_CREATE_INFO),
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };
}

// get default initialized shader stage information
VkPipelineShaderStageCreateInfo defaultPipelineShaderStageCreateInfo(VkShaderStageFlagBits shaderStage,
                                                                     VkShaderModule shaderModule){
    return VkPipelineShaderStageCreateInfo{
        .sType = STYPE(PIPELINE_SHADER_STAGE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .stage = shaderStage,
        .module = shaderModule,
        .pName = "main",
    };
}

// get default initialized vertex input stage information
VkPipelineVertexInputStateCreateInfo defaultPipelineVertexInputStateCreateInfo(){
    return VkPipelineVertexInputStateCreateInfo{
        .sType = STYPE(PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .vertexAttributeDescriptionCount = 0
    };
}

// get default initialized input assembly state information
VkPipelineInputAssemblyStateCreateInfo defaultPipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology){
    return VkPipelineInputAssemblyStateCreateInfo{
        .sType = STYPE(PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .topology = topology,
        .primitiveRestartEnable = VK_FALSE
    };
}

// get preinitialized rasterization state create info
VkPipelineRasterizationStateCreateInfo defaultPipelineRasterizationStateCreateInfo(VkPolygonMode polygonMode){
    return VkPipelineRasterizationStateCreateInfo{
        .sType = STYPE(PIPELINE_RASTERIZATION_STATE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        // If depthClampEnable is set to VK_TRUE, then fragments that are
        // beyond the near and far planes are clamped to them as opposed
        // to discarding them. This is useful in some special cases like
        // shadow maps. Using this requires enabling a GPU feature.
        .depthClampEnable = VK_FALSE,
        // If rasterizerDiscardEnable is set to VK_TRUE, then geometry
        // never passes through the rasterizer stage. This basically
        // disables any output to the framebuffer.
        .rasterizerDiscardEnable = VK_FALSE,
        // this'll decide, whether we draw a solid mesh or a wireframe or
        // just the points
        .polygonMode = polygonMode,
        // basically only one side of an object is always visible
        // so no need to draw back part
        .cullMode = VK_CULL_MODE_BACK_BIT,
        // how to decide front face
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        // The rasterizer can alter the depth values by adding a constant
        // value or biasing them based on a fragment's slope. This is
        // sometimes used for shadow mapping, but we won't be using it.
        // Just set depthBiasEnable to VK_FALSE.
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.f,
        .depthBiasClamp = 0.f,
        .depthBiasSlopeFactor = 0.f,
        // thickness of line
        .lineWidth = 1.f,
    };
};

// get preinitialized rasterization state create info
VkPipelineMultisampleStateCreateInfo defaultPipelineMultisampleStateCreateInfo(){
    return VkPipelineMultisampleStateCreateInfo{
        .sType = STYPE(PIPELINE_MULTISAMPLE_STATE_CREATE_INFO),
        .pNext = 0,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT, // msaa is off by default
        .minSampleShading = 1.f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };
}

// get preinitialized color blend attachment state
VkPipelineColorBlendAttachmentState defaultPipelineColorBlendAttachmentState(){
    return VkPipelineColorBlendAttachmentState{
        .blendEnable = VK_FALSE,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                          VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT |
                          VK_COLOR_COMPONENT_A_BIT
    };
}

// get preinitialized pipeline layout create info
VkPipelineLayoutCreateInfo defaultPipelineLayoutCreateInfo(){
    return VkPipelineLayoutCreateInfo{
        .sType = STYPE(PIPELINE_LAYOUT_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };
}

// get preinitialized pipeline dynamic state create info
VkPipelineDynamicStateCreateInfo defaultPipelineDynamicStateCreateInfo(std::vector<VkDynamicState>& dynamicStates,
                                                                       VkPipelineDynamicStateCreateFlags flags){
    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo {};
    pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipelineDynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
    pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    pipelineDynamicStateCreateInfo.flags = flags;
    return pipelineDynamicStateCreateInfo;
}
