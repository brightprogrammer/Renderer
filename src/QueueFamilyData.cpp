#include "QueueFamilyData.hpp"
#include "Device.hpp"
#include <vulkan/vulkan_core.h>

QueueFamilyData::QueueFamilyData(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface){
    // get queue family properties
    std::vector<VkQueueFamilyProperties> qfProperties;
    getPhysicalDeviceQueueFamilyProperties(physicalDevice, qfProperties);

    // check whether graphics and present queue are supported or not
    for(uint32_t qidx = 0; qidx < qfProperties.size(); qidx++){
        // check for graphics queue
        if(qfProperties[qidx].queueFlags & VK_QUEUE_GRAPHICS_BIT){
            graphicsQueueIdx = qidx;
        }

        // check for compute queue
        if(qfProperties[qidx].queueFlags & VK_QUEUE_COMPUTE_BIT){
            computeQueueIdx = qidx;
        }

        // check for transfer queue
        if(qfProperties[qidx].queueFlags & VK_QUEUE_TRANSFER_BIT){
            transferQueueIdx = qidx;
        }

        // check for surface support queue
        VkBool32 support = VK_FALSE;
        VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, qidx, surface, &support);
        if(res == VK_SUCCESS){
            if(support == VK_TRUE) surfaceSupportQueueIdx = qidx;
        }else{
            std::cerr << "[ERROR] Failed to check for Surface support" << std::endl;
            exit(1);
        }

        // if we got all queues then break
        if(graphicsQueueIdx >= 0 &&
            computeQueueIdx >= 0 &&
            transferQueueIdx >= 0 &&
           surfaceSupportQueueIdx >= 0){
            break;
        }
    }
}
