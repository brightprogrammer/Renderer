/**
 * @file      VertexInputDescription.hpp
 * @brief     Header of
 * @date      Fri Dec  3 19:10:35 2021
 * @author    Siddharth Mishra
 */

#ifndef VERTEX_INPUT_DESCRIPTION_HPP
#define VERTEX_INPUT_DESCRIPTION_HPP

#include <vector>
#include <vulkan/vulkan.h>

struct VertexInputDescription {
    std::vector<VkVertexInputBindingDescription> bindings;
    std::vector<VkVertexInputAttributeDescription> attributes;
    VkPipelineVertexInputStateCreateFlags flags = 0;
};

#endif//VERTEX_INPUT_DESCRIPTION_HPP
