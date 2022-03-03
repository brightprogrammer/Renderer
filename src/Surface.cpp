#include "Surface.hpp"
#include <vulkan/vulkan_core.h>

ReturnCode getSurfacePresentModes(VkPhysicalDevice physicalDevice,
                                  VkSurfaceKHR surface,
                                  std::vector<VkPresentModeKHR> &presentModes){
    // get surface format count
    uint32_t count = 0;
    VkResult res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                                             &count, nullptr);
    if(res != VK_SUCCESS){
        return ReturnCode::Failed;
    }

    // get surface formats
    presentModes.resize(count);
    res = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface,
                                                    &count, presentModes.data());
    if(res == VK_SUCCESS){
        return ReturnCode::Success;
    }else if(res == VK_INCOMPLETE){
        return ReturnCode::Incomplete;
    }else{
        return ReturnCode::Failed;
    }
}

// get surface formats
ReturnCode getSurfaceFormats(VkPhysicalDevice physicalDevice,
                                  VkSurfaceKHR surface,
                                  std::vector<VkSurfaceFormatKHR> &surfaceFormats){
    // get surface format count
    uint32_t count = 0;
    VkResult res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
                                                        &count, nullptr);
    if(res != VK_SUCCESS){
        std::cerr << "[ERROR] Failed to get surface format count. Returned : "
                  << "\"" << VkResultString(res) << "\"" << std::endl;
        exit(1);
    }

    // get surface formats
    surfaceFormats.resize(count);
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface,
                                               &count, surfaceFormats.data());
    if(res == VK_SUCCESS){
        return ReturnCode::Success;
    }else if(res == VK_INCOMPLETE){
        return ReturnCode::Incomplete;
    }else{
        return ReturnCode::Failed;
    }
}



