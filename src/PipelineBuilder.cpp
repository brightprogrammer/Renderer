#include "PipelineBuilder.hpp"
#include "Initializers.hpp"
#include "VkResultString.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>

// build pipeline with pre initialized details
VkPipeline PipelineBuilder::buildPipeline(VkDevice device, VkRenderPass renderPass){
    // make viewport state from stared viewport and scissor
    VkPipelineViewportStateCreateInfo viewportState{
        .sType = STYPE(PIPELINE_VIEWPORT_STATE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

    // setup dummy color blend state
    // this is basically doing no blending
    VkPipelineColorBlendStateCreateInfo colorBlending{
        .sType = STYPE(PIPELINE_COLOR_BLEND_STATE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment
    };

    VkGraphicsPipelineCreateInfo pipelineInfo{
        .sType = STYPE(GRAPHICS_PIPELINE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .stageCount = static_cast<uint32_t>(shaderStages.size()),
        .pStages = shaderStages.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depthStencil,
        .pColorBlendState = &colorBlending,
        .layout = pipelineLayout,
        .renderPass = renderPass,
        .subpass = 0, // first first subpass in the given renderpass
        .basePipelineHandle = VK_NULL_HANDLE
    };

    // create pipeline
    VkPipeline pipeline;
    VkResult res = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
    if(res != VK_SUCCESS){
        std::cerr << "[ERROR] Failed to create graphics pipeline. Returned \"" << VkResultString(res) << "\"" << std::endl;
        return VK_NULL_HANDLE;
    } else return pipeline;
}
