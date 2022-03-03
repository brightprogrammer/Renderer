#ifndef FRAME_DATA_HPP
#define FRAME_DATA_HPP

#include "Common.hpp"
#include "AllocatedBuffer.hpp"

struct FrameData{
    // synchronization per frame
    VkSemaphore renderSemaphore, presentSemaphore;
    VkFence renderFence;

    // commands per frame
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    // camera data (uniform data) per frame
    AllocatedBuffer cameraBuffer;
    VkDescriptorSet globalDescriptorSet;
};

#endif//FRAME_DATA_HPP
