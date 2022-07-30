#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "Common.hpp"
#include <vulkan/vulkan_core.h>

/**
 * @brief Enumerates physical device and returns it in the given vector.
 * @param[in] instance to be used to get device list.
 * @param[out] physicalDevices will contain enumerated handles of physical devices present on host.
 * @return SUCCESS on success.
 * @return INCOMPLETE if less than total number of physical devices
 * is returned.
 * @return FAILED on failure.
 */
ReturnCode enumeratePhysicalDevices(VkInstance instance,
                         std::vector<VkPhysicalDevice> &physicalDevices);

/**
 * @brief Get physical device queue family properties in given vector.
 * @param[in] physicalDevice to get queue families from.
 * @param[out] qfProperties will contain queue family propertie.
 */
void getPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice physicalDevice,
                                            std::vector<VkQueueFamilyProperties>& qfProperties);

/**
 * @brief Get device extension properties for given physical device.
 * @param[in] physicalDevice to get extension properties.
 * @param[out] extensionProperties  will contain fetched extension properties,
 * @return SUCCESS on success.
 * @return INCOMPLETE if less than total number of extension properties is returned.
 * @return FAILED on failure.
 */
ReturnCode getDeviceExtensionProperties(VkPhysicalDevice physicalDevice,
                                        std::vector<VkExtensionProperties>& extensionProperties);

/**
 * @brief Get device extension properties for given physical device.
 * @param[in] physicalDevice to get extension properties.
 * @param[in] deviceExtensions must be requested device extensions.
 * @return SUCCESS if all required extensions are present.
 * @return FAILED if any one of the required extensions are not present.
 */
ReturnCode checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice,
                                       const std::vector<const char*>& deviceExtensions);

#endif//DEVICE_HPP
