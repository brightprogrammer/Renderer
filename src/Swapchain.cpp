#include "Swapchain.hpp"

// get images from swapchain
ReturnCode getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain,
                              std::vector<VkImage>& swapchainImages){
    // get number of swapchain images
    uint32_t count = 0;
    VkResult res = vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr);
    if(res != VK_SUCCESS){
        std::cerr << "[ERROR] Failed to get number of images in swapchain. Returned : "
                  << "\"" << VkResultString(res) << "\"" << std::endl;
        exit(1);
    }

    // get swapchain images
    swapchainImages.clear();
    swapchainImages.resize(count);
    res =  vkGetSwapchainImagesKHR(device, swapchain, &count, swapchainImages.data());

    if(res == VK_SUCCESS){
        return SUCCESS;
    }else if(res == VK_INCOMPLETE){
        return INCOMPLETE;
    }else{
        return FAILED;
    }
 }
