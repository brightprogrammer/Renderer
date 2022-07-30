#ifndef SURFACE_DETAILS_HPP
#define SURFACE_DETAILS_HPP

#include "Common.hpp"
#include "ReturnCode.hpp"
#include <vulkan/vulkan_core.h>

/// contains details about surface
struct SurfaceDetails{
    /// surface used
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    /// physical device used
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    /// surface capabilities for given physical device and surface
    VkSurfaceCapabilitiesKHR capabilities;
    /// surface formats for given physical device and surface
    std::vector<VkSurfaceFormatKHR> formats;
    /// surface present for given physical device and surface
    std::vector<VkPresentModeKHR> presentModes;
};

/***********************************************************************************
 * @brief Get SurfaceDetails from given physicalDevice and surface
 *
 * @param physicalDevice [in] to be used for querying
 * @param surface [in] to be used for querying
 * @param surfaceDetails [out] will be filled with surface details
 * @return SUCCESS on success
 * @return FAILED on failure
 ***********************************************************************************/
ReturnCode getSurfaceDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                             SurfaceDetails& surfaceDetails);

#endif//SURFACE_DETAILS_HPP
