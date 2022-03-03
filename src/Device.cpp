#include "Device.hpp"
#include "ReturnCode.hpp"
#include <vulkan/vulkan_core.h>

// get physical device list
ReturnCode enumeratePhysicalDevices(VkInstance instance,
                                    std::vector<VkPhysicalDevice> &physicalDevices){
    // get number of physical devices
    uint32_t c = 0;
    if(vkEnumeratePhysicalDevices(instance, &c, nullptr) != VK_SUCCESS){
        return ReturnCode::Failed;
    }

    // check if there are any devices present or not
    if(c == 0){
        return ReturnCode::Failed;
    }

    // get physical device handles
    physicalDevices.resize(c);
    VkResult res = vkEnumeratePhysicalDevices(instance, &c, physicalDevices.data());
    if(res == VK_SUCCESS){
        return ReturnCode::Success;
    } else if (res == VK_INCOMPLETE){
        return ReturnCode::Incomplete;
    } else {
        return ReturnCode::Failed;
    }
}

// get queue family properties
void getPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice,
                                            std::vector<VkQueueFamilyProperties>& qfProperties){
    // get queue family count
    uint32_t c = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &c, nullptr);

    // get queue family properties
    qfProperties.resize(c);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &c, qfProperties.data());
}

// get extension properties for given physical device
ReturnCode getDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                        std::vector<VkExtensionProperties>& extensionProperties){
    // check if required physical device extensions are supported or notx
    // get extension count
    uint32_t c;
    VkResult res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &c, nullptr);
    if(res != VK_SUCCESS){
        return ReturnCode::Failed;
    }

    // get extension properties
    extensionProperties.resize(c);
    res = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &c, extensionProperties.data());
    if(res == VK_SUCCESS){
        return ReturnCode::Success;
    }else if(res == VK_INCOMPLETE){
        return ReturnCode::Incomplete;
    }else{
        return ReturnCode::Failed;
    }
}

// check whether given extensions are supported by given physical device
ReturnCode checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice,
                                       const std::vector<const char*>& deviceExtensions){
    // get extension properties
    std::vector<VkExtensionProperties> extensionProperties;
    ReturnCode retCode = getDeviceExtensionProperties(physicalDevice, extensionProperties);
    if(retCode == ReturnCode::Failed){
        return ReturnCode::Failed;
    }else if(retCode == ReturnCode::Incomplete){
        getDeviceExtensionProperties(physicalDevice, extensionProperties);
    }

    // check for extension names
    for(auto extName : deviceExtensions){
        bool found = false;
        for(const auto& extProp : extensionProperties){
            if(strcmp(extProp.extensionName, extName) == 0){
                found = true;
            }
        }
        if(!found){
            return ReturnCode::Failed;
        }
    }

    return ReturnCode::Success;
}
