/**
 * @file      PipelineConfig.hpp
 * @brief     Required for creating pipelines using pipeline manager
 * @date      Thu Dec  2 11:20:28 2021
 * @author    Siddharth Mishra
 */

#ifndef PIPELINE_CONFIG_HPP
#define PIPELINE_CONFIG_HPP

#include "Common.hpp"


struct PipelineConfig{
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;

    /// optional parameter.
    /// this will automatically be set to device handle
    /// used to create a pipeline.
    VkDevice device;
};

#endif//PIPELINE_CONFIG_HPP
