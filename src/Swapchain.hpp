#ifndef SWAPCHAIN_HPP
#define SWAPCHAIN_HPP

#include "Common.hpp"

/**
 * @brief Get swapchain images from given swapchain.
 * @param[in] device used to create swapchain.
 * @param[in] swapchain that created images.
 * @param[out] swapchainImages created by swapchain.
 * @return SUCCESS on success.
 * @return INCOMPLETE if less number of swapchain images are returned than
 * total number.
 * @return FAILED on failure.
 */
ReturnCode getSwapchainImages(VkDevice device, VkSwapchainKHR swapchain,
                              std::vector<VkImage>& swapchainImages);

#endif//SWAPCHAIN_HPP
