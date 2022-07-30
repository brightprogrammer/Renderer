#include "ExtensionsAndLayers.hpp"
#include "ReturnCode.hpp"
#include <cstring>

// get names of all available layers
ReturnCode checkIfLayerIsAavailable(const char* layerName){
    // get layer count
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    // get layer properties
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // to keep track
    bool layerFound = false;

    // check for availability
    for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
            layerFound = true;
            break;
        }
    }

    // check available
    if (!layerFound) {
        return FAILED;
    }else{
        return SUCCESS;
    }
}

// get names of all available extensions
ReturnCode checkIfExtensionIsAvailable(const char* extensionName){
    // get extension count
    uint32_t extensionCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    // get layer properties
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

    // to keep track
    bool extensionFound = false;

    // check for availability
    for (const auto& extensionProperties : availableExtensions) {
        if (strcmp(extensionName, extensionProperties.extensionName) == 0) {
            extensionFound = true;
            break;
        }
    }

    // check available
    if (!extensionFound) {
        return FAILED;
    }else{
        return SUCCESS;
    }
}

// get reqired extensions and layers after all checks
ReturnCode getExtensionsAndLayers(SDL_Window *window,
                           std::vector<const char *> &layerNames,
                           std::vector<const char *> &extensionNames) {
    if(validationEnabled){
        // check if requred layers are present or not
        for (uint i = 0; i < requiredLayers.size(); i++) {
            // if not found then return failed
            if (checkIfLayerIsAavailable(requiredLayers[i]) != SUCCESS) {
                std::cerr << "[WARNING] fn(getExtensionAndLayers) : Layer \""
                          << requiredLayers[i] << "\" requested but not found"
                          << std::endl;

                return FAILED;
            }else{
                // since required layers is static, we can give pointers stored in it
                // without doing a strcmp
                layerNames.push_back(requiredLayers[i]);
            }
        }
    }

    // surface extension names to be enabled
    uint32_t extCount;
    if (SDL_Vulkan_GetInstanceExtensions(window, &extCount, nullptr) == SDL_FALSE) {
        // error
        std::cerr << "[ERROR] fn(getExntensionsAndLayers) : Failed to get number of surface extensions ["
                  << SDL_GetError() << "]" << std::endl;

        // failed
        return FAILED;
    } else {
        extensionNames.resize(extCount);
        if (SDL_Vulkan_GetInstanceExtensions(window, &extCount, extensionNames.data()) == SDL_FALSE) {
            // print error
            std::cerr << "[ERROR] fn(getExntensionsAndLayers) : Failed to get names of required surface extensions ["
                      << SDL_GetError() << "]" << std::endl;

            // failed
            return FAILED;
        }
    }

    // check if requested extensions are present or not
    for(uint i = 0; i < requiredExtensions.size(); i++){
        // if not present then error
        if(checkIfExtensionIsAvailable(requiredExtensions[i]) != SUCCESS){
            std::cerr << "[WARNING] fn(getExtensionAndLayers) : Extension \""
                      << requiredExtensions[i] << "\" requested but not found"
                      << std::endl;

            return FAILED;
        }else{
            // since requiredExtensions is static we can just give in pointer
            // to strings in it, instead of copying first and then storing!
            extensionNames.push_back(requiredExtensions[i]);
        }
    }

    // everything completed
    return SUCCESS;
}
