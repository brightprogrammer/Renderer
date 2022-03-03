#include "PipelineManager.hpp"
#include "PipelineConfig.hpp"
#include "Utils/ResourceManager.hpp"
#include "VkResultString.hpp"

#include <cstdint>
#include <cassert>

#include <vulkan/vulkan_core.h>

PipelineManager::PipelineManager(VkDevice device){
    
}

void PipelineManager::preparePipelineCreateInfo(const PipelineConfig* config, VkGraphicsPipelineCreateInfo& createInfo){
    // make viewport state from stared viewport and scissor
    VkPipelineViewportStateCreateInfo viewportState{
        .sType = STYPE(PIPELINE_VIEWPORT_STATE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = &config->viewport,
        .scissorCount = 1,
        .pScissors = &config->scissor
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
        .pAttachments = &config->colorBlendAttachment
    };

    // create info
    createInfo = VkGraphicsPipelineCreateInfo{
        .sType = STYPE(GRAPHICS_PIPELINE_CREATE_INFO),
        .pNext = nullptr,
        .flags = 0,
        .stageCount = static_cast<uint32_t>(config->shaderStages.size()),
        .pStages = config->shaderStages.data(),
        .pVertexInputState = &config->vertexInputInfo,
        .pInputAssemblyState = &config->inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &config->rasterizer,
        .pMultisampleState = &config->multisampling,
        .pColorBlendState = &colorBlending,
        .layout = config->pipelineLayout,
        .renderPass = config->renderPass,
        .subpass = 0, // first first subpass in the given renderpass
        .basePipelineHandle = VK_NULL_HANDLE
    };
}

// build pipeline with pre initialized details
VkPipeline PipelineManager::buildPipeline(VkDevice device, const char* configName){
    PipelineConfig *config = configs.get(configName);
    assert((config != nullptr) && "FAILED TO GET PIPELINE CONFIG");

    // prepare create info
    VkGraphicsPipelineCreateInfo pipelineInfo;
    preparePipelineCreateInfo(config, pipelineInfo);
    config->device = device;

    // create pipeline
    VkPipeline pipeline;
    VkResult res = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
    if(res != VK_SUCCESS){
        std::cerr << "[ERROR] Failed to create graphics pipeline. Returned \"" << VkResultString(res) << "\"" << std::endl;
        return VK_NULL_HANDLE;
    } else {
        pipelines.add(configName, pipeline);
        return pipeline;
    }
}

// build all pipelines
void PipelineManager::buildPipelines(VkDevice device){
    // allocate space
    std::vector<VkGraphicsPipelineCreateInfo> createInfos(configs.resources.size());
    std::vector<VkPipeline> pipelineHandles(configs.resources.size());


    // prepare create infos
    for(size_t i = 0; i < configs.resources.size(); i++){
        auto& config = configs.resources[i];
        config.handle.device = device;

        VkGraphicsPipelineCreateInfo pipelineInfo;
        preparePipelineCreateInfo(&config.handle, pipelineInfo);

        createInfos[i] = pipelineInfo;
    }

    // create pipelines
    VKCHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, createInfos.size(), createInfos.data(), nullptr, pipelineHandles.data()));

    // add pipelines to resource manager
    for(size_t i = 0; i < configs.resources.size(); i++){
        pipelines.add(configs.resources[i].name, pipelineHandles[i]);
    }
}

// get pipeline handle
VkPipeline PipelineManager::getPipeline(const char *name){
    VkPipeline *pipeline = pipelines.get(name);
    assert(pipeline != nullptr && "FAILED TO FIND QUERIED PIPELINE");
    return *pipeline;
}

// get previously added config
PipelineConfig* PipelineManager::getConfig(const char *name){
    PipelineConfig* pipelineConfig = configs.get(name);
    assert(pipelineConfig && "FAILED TO GET QUERIED PIPELINE CONFIG");
    return pipelineConfig;
}

// add another pipeline configuration
void PipelineManager::addConfig(const char* name, const PipelineConfig &config){
    configs.add(name, config);
}

// destroy pipeline
void PipelineManager::destroyPipeline(const char *pipelineName){
    VkPipeline pipeline = getPipeline(pipelineName);
    VkDevice device = getConfig(pipelineName)->device;
    vkDestroyPipeline(device, pipeline, nullptr);
}
