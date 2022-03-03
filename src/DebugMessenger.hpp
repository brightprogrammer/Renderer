#ifndef DEBUG_MESSENGER_HPP
#define DEBUG_MESSENGER_HPP

#include "Common.hpp"
#include <vulkan/vulkan_core.h>

class DebugMessenger{
public:
    // create a new debug messenger with default create info
    ReturnCode create(VkInstance instance);

    // create a new debug messenger with given create info
    ReturnCode create(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);

    // destroy debug messenger
    ReturnCode destroy(VkInstance instance);

    // default debug utils messenger create info
    static VkDebugUtilsMessengerCreateInfoEXT defaultCreateInfo();
private:
    // handle to debug messenger
    VkDebugUtilsMessengerEXT handle;

    // default debug callback function
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
                  void *userData);

    // get's the function pointer to create debug utils messenger and creates a
    // debug util's messenger for us
    ReturnCode createDebugUtilsMessengerEXT(
        VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugUtilsMessengerEXT *pDebugMessenger);

    // get's the function pointer to destroy debug utils messenger
    ReturnCode destroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks *pAllocator);
};

#endif//DEBUG_MESSENGER_HPP
