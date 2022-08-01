#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <vulkan/vulkan.h>

// Materials are applied to objects to give them good looks
struct Material {
    // light data
    float roughness = 1.f;

    // pipeline decides shaders, region, color blending and all
    VkPipeline pipeline;

    // pipeline layout decides what data must be sent to pipeline
    VkPipelineLayout pipelineLayout;
};


#endif // MATERIAL_H_
