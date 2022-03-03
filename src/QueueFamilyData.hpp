/**
 * @file      QueueFamilyData.hpp
 * @brief     Header of
 * @date      Fri Nov 26 23:44:05 2021
 * @author    Siddharth Mishra
 * @copyright BSD-3-Clause
 * 
 * This module
 */

#ifndef QUEUE_FAMILY_DATA_HPP
#define QUEUE_FAMILY_DATA_HPP

#include "Common.hpp"

/// Contains queue family data in a sorted manner
struct QueueFamilyData{
    /// default constructor, does nothing
    QueueFamilyData() = default;

    /// fill data using physicalDevice
    QueueFamilyData(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

    /// index of graphics queue
    uint32_t graphicsQueueIdx = -1;
    /// index of compute queue
    uint32_t computeQueueIdx = -1;
    /// index of transfer queue
    uint32_t transferQueueIdx = -1;
    /// index of queue family that supports presenting rendered images to surface
    uint32_t surfaceSupportQueueIdx = -1;
};

#endif//QUEUE_FAMILY_DATA_HPP
