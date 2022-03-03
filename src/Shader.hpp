#ifndef SHADER_HPP
#define SHADER_HPP

#include "Common.hpp"

/**
 * @brief Load shader code and create shader module.
 * @param[in] filepath is the path of shader module to load.
 * @param[in] device to create shader module for.
 * @param[out] shaderModule is the created shader module.
 * @return ReturnCode::Success on success.
 * @return ReturnCode::Failure on failure.
 */
ReturnCode loadShaderModule(const char* filepath, VkDevice device,
                            VkShaderModule& shaderModule);

#endif//SHADER_HPP
