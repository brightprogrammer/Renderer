#include "SurfaceDetails.hpp"
#include "ReturnCode.hpp"
#include "Surface.hpp"

// get surface details
ReturnCode getSurfaceDetails(VkPhysicalDevice physicalDevice,VkSurfaceKHR surface,
                             SurfaceDetails& surfaceDetails){
    // cache surface and physical device
    surfaceDetails.surface = surface;
    surfaceDetails.physicalDevice = physicalDevice;

    // get surface capabilities
    VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface,
                                                             &surfaceDetails.capabilities);
    if(res != VK_SUCCESS){
        return FAILED;
    }

    // get surface present modes
    ReturnCode retCode = getSurfacePresentModes(physicalDevice, surface,
                                                surfaceDetails.presentModes);
    if(retCode == INCOMPLETE){
        getSurfacePresentModes(physicalDevice, surface, surfaceDetails.presentModes);
    }else if(retCode == FAILED){
        return FAILED;
    }

    // get surface formats
    retCode = getSurfaceFormats(physicalDevice, surface,
                                surfaceDetails.formats);
    if(retCode == INCOMPLETE){
        getSurfaceFormats(physicalDevice, surface, surfaceDetails.formats);
    }else if(retCode == FAILED){
        return FAILED;
    }

    return SUCCESS;
}
