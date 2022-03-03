#include "DebugMessenger.hpp"
#include "ReturnCode.hpp"
#include <vulkan/vulkan_core.h>

// setup debug messenger for this renderer
ReturnCode DebugMessenger::create(VkInstance instance){
    VkDebugUtilsMessengerCreateInfoEXT createInfo = defaultCreateInfo();
    if (createDebugUtilsMessengerEXT(instance, &createInfo,
                                     nullptr, &handle) != ReturnCode::Success) {
        return ReturnCode::Failed;
    }

    return ReturnCode::Success;
}

// create debug utils messenger for given create info
ReturnCode DebugMessenger::create(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo){
    if (createDebugUtilsMessengerEXT(instance, pCreateInfo,
                                     nullptr, &handle) != ReturnCode::Success) {
        return ReturnCode::Failed;
    }

    return ReturnCode::Success;
}

// destroy debug messenger
ReturnCode DebugMessenger::destroy(VkInstance instance){
    if(destroyDebugUtilsMessengerEXT(instance, handle, nullptr) != ReturnCode::Success){
        return ReturnCode::Failed;
    }
    return ReturnCode::Success;
}

// debug callback function
VKAPI_ATTR VkBool32 VKAPI_CALL
DebugMessenger::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
              void *userData) {
    // print severity
    if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) std::cerr << "\nValidationLayer [ERROR] : ";
    else if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) std::cerr << "\nValidationLayer [WARNING] : ";
    else if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) std::cerr << "\nValidationLayer [INFO] : ";
    else std::cerr << "\nValidationLayer [VERBOSE] : ";

    // print the message
    std::cerr << callbackData->pMessage << std::endl;

    // always return false
    return VK_FALSE;
}


// get's the function pointer to create debug utils messenger and creates a
// debug util's messenger for us
ReturnCode DebugMessenger::createDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
    // get function pointer
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");

    // if we got the pointer
    if (func != nullptr) {
        if (func(instance, pCreateInfo, nullptr, pDebugMessenger) != VK_SUCCESS) {
            std::cerr << "Failed to create debug messenger" << std::endl;
            return ReturnCode::Failed;
        }else{ // only success case here
            return ReturnCode::Success;
        }
    } else {
        std::cerr << "Failed to get \"vkCreateDebugUtilsMessengerEXT\" function pointer"
                  << std::endl;
        return ReturnCode::Failed;
    }
}


// get's the function pointer to destroy debug utils messenger
ReturnCode DebugMessenger::destroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator) {
    // get function pointer
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");

    // if we got the pointer
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator); 
        return ReturnCode::Success;
    } else {
        std::cerr << "Failed to get \"vkDestroyDebugUtilsMessengerEXT\" function pointer"
                  << std::endl;
        return ReturnCode::Failed;
    }
}

// default create info
VkDebugUtilsMessengerCreateInfoEXT DebugMessenger::defaultCreateInfo(){
    // null initialize all fields
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};

    // structure type
    createInfo.sType = STYPE(DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT);

    // we need to see only warning and error messages
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    // all types of messages
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    // the callback function
    createInfo.pfnUserCallback =
        reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(debugCallback);

    // no user data
    createInfo.pUserData = nullptr;

    return createInfo;
}
