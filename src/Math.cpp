#include "Math.hpp"
#include <cmath>

glm::vec3 sphericalToCartesian(float radius, float theta, float phi){
    glm::vec3 pos;
    pos.x = radius*sin(phi)*cos(theta);
    pos.z = radius*sin(phi)*sin(theta);
    pos.y = radius*cos(phi);

    return pos;
}
