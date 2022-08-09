#ifndef UNIFORM_DATA_HPP
#define UNIFORM_DATA_HPP

#include <glm/glm.hpp>

#define MAX_LIGHTS 16

struct PointLight {
    glm::vec4 position{0.f};
    glm::vec4 color{1.f};
};

struct DirectionalLight {
    glm::vec4 direction;
    glm::vec4 color;
};

// represents a uniform buffer object
struct UniformData {
    glm::mat4 projectionMatrix{1.f};
    glm::mat4 viewMatrix{1.f};
    glm::vec4 ambient{1.f};
    glm::vec3 viewPosition{0.f};
    uint32_t numPointLights = 0;
    PointLight pointLights[MAX_LIGHTS];
};

#endif//UNIFORM_DATA_HPP
