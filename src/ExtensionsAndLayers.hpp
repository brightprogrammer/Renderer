#include <vector>
#include <cstring>
#include <cstdint>
#include <iostream>

#include "ReturnCode.hpp"
#include "VkResultString.hpp"

#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#ifndef EXTENSION_AND_LAYERS_HPP
#define EXTENSION_AND_LAYERS_HPP

// if not debug is not defined then validation is enabled
// else validation is disabled
#ifndef NDEBUG
static bool validationEnabled = true;
static std::vector<const char *> requiredExtensions = {
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
static std::vector<const char *> requiredLayers = {
    "VK_LAYER_KHRONOS_validation"};
#else
static bool validationEnabled = false;
static std::vector<const char *> requiredExtensions = {};
static std::vector<const char *> requiredLayers = {};
#endif

/****************************************************************************************************************
 * @brief Get names of required extension names and layer names that are present on
 * host system. This function will clear the output vectors and then add names
 * to the output vectors.
 *
 * @param window [in] is window to get platform specific surface extension names.
 * @param layerNames [out] will contain the names of layers required and present.
 * @param extensionNames [out] will contain names of extensions required and present.
 *
 * @return FAILED if some error occured.
 * @return INCOMPLETE if failed to find all requested layer or extensions.
 * @return SUCCESS if all requested layer and extesions were found.
 ***************************************************************************************************************/
ReturnCode getExtensionsAndLayers(SDL_Window *window,
								  std::vector<const char *> &layerNames,
								  std::vector<const char *> &extensionNames);

#endif//EXTENSION_AND_LAYERS_HPP
