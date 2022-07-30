#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "Common.hpp"
#include <vulkan/vulkan_core.h>

/**
 * @brief Get surface present modes for given physical device and surface.
 * @param[in] physicalDevice to be used.
 * @param[in] surface to query information for.
 * @param[out] presentModes filled with queried present modes.
 * @return SUCCESS on success.
 * @return INCOMPLETE if number of present modes returned is less than
 * total number of present modes.
 * @return FAILED on failure.
 */
ReturnCode getSurfacePresentModes(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                                 std::vector<VkPresentModeKHR>& presentModes);

/**
 * @brief Get surface formats for given physical device and surface.
 * @param[in] physicalDevice to be used.
 * @param[in] surface to query information for.
 * @param[out] surfaceFormats filled with queried surface formats.
 * @return SUCCESS on success.
 * @return INCOMPLETE if number of surface formats returned is less
 * than total number of present modes.
 * @return FAILED on failure.
 */
ReturnCode getSurfaceFormats(VkPhysicalDevice physicalDevice,
                             VkSurfaceKHR surface,
                             std::vector<VkSurfaceFormatKHR> &surfaceFormats);

#endif//SURFACE_HPP
