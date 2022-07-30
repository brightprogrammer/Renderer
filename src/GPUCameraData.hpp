#ifndef GPU_CAMERA_DATA_HPP
#define GPU_CAMERA_DATA_HPP

#include <glm/glm.hpp>

struct GPUCameraData{
    glm::mat4 view;
    glm::mat4 projection;
};

#endif//GPU_CAMERA_DATA_HPP
