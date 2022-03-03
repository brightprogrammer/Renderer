/**
 * @file      Initializers.hpp
 * @brief     Contiains helper functions to help initialize some structs with some default values.
 * @date      Sun Nov 28 23:43:18 2021
 * @author    Siddharth Mishra
 * @copyright All Rights Reserved.
 * 
 * This module
 */


#ifndef INITIALIZERS_HPP
#define INITIALIZERS_HPP

#include "Common.hpp"
#include <vulkan/vulkan_core.h>

/**
 * @brief Get default initialized image create info.
 * @param[in] format of image.
 * @param[in] usageFlags Is it depth image or color attachment image or something else?.
 * @param[in] extent or size of image.
 * @return preinitialized VkImageCreateInfo struct.
 */
VkImageCreateInfo defaultImageCreateInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);

/**
 * @brief Get default initialized image view create info.
 * @param[in] format of depth image.
 * @param[in] image to create image view for.
 * @param[in] aspectFlags which aspects of image to write to.
 * @return preintialized VkImageViewCreateInfo struct.
 */
VkImageViewCreateInfo defaultImageViewCreateInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);

/**
 * @brief Get default initialized command pool create info.
 * @param[in] queueIdx is queue family index that the command buffers will be submitted to.
 * @param[in] flags for command pool creation.
 * @return preinitialized VkCommandPoolCreateInfo struct.
 */
VkCommandPoolCreateInfo defaultCommandPoolCreateInfo(uint32_t queueIdx, VkCommandPoolCreateFlags flags);

/**
 * @brief Get default initialized command buffer allocate info.
 * @param[in] commandPool to allocate command buffers from.
 * @param[in] count The number of command buffers to allocate.
 * @return preinitialized VkCommandBufferAllocateInfo struct.
 */
VkCommandBufferAllocateInfo defaultCommandBufferAllocateInfo(VkCommandPool commandPool, uint32_t count);

/**
 * @brief Get default initialized semaphore create info.
 * @return preinitialized VkSemaphoreCreateInfo struct.
 */
VkSemaphoreCreateInfo defaultSemaphoreCreateInfo();

/**
 * @brief Get default initialized fence create info.
 * @param[in] flags for fence creation.
 * @return preinitialized VkFenceCreateInfo struct.
 */
VkFenceCreateInfo defaultFenceCreateInfo(VkFenceCreateFlags flags);

/**
 * @brief Get default initialized shader stage create info.
 * @param[in] shaderStage that the shader module belongs to.
 * @param[in] shaderModule is the VkShaderModule created from spirv bytecode.
 * @return preinitialized VkPipelineShaderStageCreateInfo struct.
 */
VkPipelineShaderStageCreateInfo defaultPipelineShaderStageCreateInfo(VkShaderStageFlagBits shaderStage,
                                                                     VkShaderModule shaderModule);

/**
 * @brief Get default initialized shader state create info.
 * @return preinitialized VkPipelineVertexInputStateCreateInfo struct.
 */
VkPipelineVertexInputStateCreateInfo defaultPipelineVertexInputStateCreateInfo();

/**
 * @brief Get default initialized input assembly state create info.
 * @param[in] topology to be used, point, line or triangles.
 * @return preinitialized VkPipelineInputAssemblyStateCreateInfo struct.
 */
VkPipelineInputAssemblyStateCreateInfo defaultPipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology);

/**
 * @brief Get default initialized rasterization state create info.
 * @param[in] polygonMode to be used for rasterization.
 * @return preinitialized VkPipelineRasterizationStateCreateInfo struct.
 */
VkPipelineRasterizationStateCreateInfo defaultPipelineRasterizationStateCreateInfo(VkPolygonMode polygonMode);

/**
 * @brief Get default initialized multisample state create info.
 * @return preinitialized VkPipelineMultisampleStateCreateInfo struct.
 */
VkPipelineMultisampleStateCreateInfo defaultPipelineMultisampleStateCreateInfo();

/**
 * @brief Get default initialized depth stencil state create info.
 * @param[in] bDepthTest Should I enable depth test?
 * @param[in] bDepthWrite Should I write depth values too?
 * @param[in] compareOp What comparision operation should I use to compare z values
 * @return preinitialized VkPipelineDepthStencilStateCreateInfo struct
 */
VkPipelineDepthStencilStateCreateInfo defaultPipelineDepthStencilStateCreateInfo(bool bDepthTest, bool bDepthWrite, VkCompareOp compareOp);

/**
 * @brief Get default initialized pipeline color blend attachment state.
 * @return preinitialized VkPipelineColorBlendAttachmentState struct.
 */
VkPipelineColorBlendAttachmentState defaultPipelineColorBlendAttachmentState();

/**
 * @brief Get default initialized pipeline layout create info.
 * @return preinitialized VkPipelineLayoutCreateInfo struct.
 */
VkPipelineLayoutCreateInfo defaultPipelineLayoutCreateInfo();

#endif//INITIALIZERS_HPP
