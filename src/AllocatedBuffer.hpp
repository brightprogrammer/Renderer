/**
 * @file      AllocatedBuffer.hpp
 * @brief     Header of
 * @date      Fri Dec  3 18:49:28 2021
 * @author    Siddharth Mishra
 */

#ifndef ALLOCATED_BUFFER_HPP
#define ALLOCATED_BUFFER_HPP

#include "vk_mem_alloc.h"
#include "Common.hpp"

struct AllocatedBuffer{
    VkBuffer buffer;
    VmaAllocation allocation;
};

#endif//ALLOCATED_BUFFER_HPP
