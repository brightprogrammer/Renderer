#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <cstdint>

#include <vulkan/vulkan.h>
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL.h>

#include "ReturnCode.hpp"
#include "VkResultString.hpp"
#include "ExtensionsAndLayers.hpp"

#define STYPE(x) VK_STRUCTURE_TYPE_##x

// we want to immediately abort when there is an error.
// In normal engines this would give an error message to the user,
// or perform a dump of state.
using namespace std;
#define VKCHECK(x)                                                      \
    do                                                                  \
    {                                                                   \
        VkResult err = x;                                               \
        if (err)                                                        \
        {                                                               \
            std::cerr <<"[ERROR] " << #x << " = " << VkResultString(err) << std::endl; \
            abort();                                                    \
        }                                                               \
    } while (0)
#endif//COMMON_HPP
