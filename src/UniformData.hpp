#ifndef UNIFORM_DATA_HPP
#define UNIFORM_DATA_HPP

#include <glm/glm.hpp>

// represents a uniform buffer object
struct UniformData {
    glm::mat4 projectionViewMatrix;
    glm::vec4 ambientLightColor = {1.f, 1.f, 1.f, 0.02}; // w is intensity
    glm::vec3 lightPosition = {1, 1, 1};
    float padding1;
    glm::vec4 lightColor = {1, 1, 1, 1}; // w component stores light intensity
    glm::vec3 viewPosition = {0, 0, 0};
    float specularStrength = 4;
};

#endif//UNIFORM_DATA_HPP
