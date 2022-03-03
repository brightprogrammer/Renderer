#include "Swapchain.hpp"

// get images from swapchain
ReturnCode getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain,
                              std::vector<VkImage>& swapchainImages){
    // get surface format count
    uint32_t count = 0;
    VkResult res = vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr);
    if(res != VK_SUCCESS){
        std::cerr << "[ERROR] Failed to get number of images in swapchain. Returned : "
                  << "\"" << VkResultString(res) << "\"" << std::endl;
        exit(1);
    }

    // get surface formats
    swapchainImages.resize(count);
    res =  vkGetSwapchainImagesKHR(device, swapchain, &count, swapchainImages.data());
    if(res == VK_SUCCESS){
        return ReturnCode::Success;
    }else if(res == VK_INCOMPLETE){
        return ReturnCode::Incomplete;
    }else{
        return ReturnCode::Failed;
    }
 }
