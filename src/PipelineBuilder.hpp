#ifndef PIPELINE_BUILDER_HPP
#define PIPELINE_BUILDER_HPP

#include "Common.hpp"
#include <vulkan/vulkan_core.h>

struct PipelineBuilder {
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VkPipelineLayout pipelineLayout;
    VkPipelineDepthStencilStateCreateInfo depthStencil;

    /**
     * @brief Build render pipeline after filling all information in the builder.
     * @param[in] device to build pipeline for.
     * @param[in] renderPass to create pipeline for.
     * @return VkPipeline handle of created pipeline.
     */
    VkPipeline buildPipeline(VkDevice device, VkRenderPass renderPass);
};

#endif//PIPELINE_BUILDER_HPP
