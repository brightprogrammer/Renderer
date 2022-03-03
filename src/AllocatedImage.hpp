/**
 * @file      AllocatedImage.hpp
 * @brief     Header of
 * @date      Sat Dec  4 18:27:38 2021
 * @author    Siddharth Mishra
 */

#ifndef ALLOCATED_IMAGE_HPP
#define ALLOCATED_IMAGE_HPP

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

struct AllocatedImage{
    VkImage image;
    VmaAllocation allocation;
};

#endif//ALLOCATED_IMAGE_HPP
