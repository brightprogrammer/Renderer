#ifndef PUSH_DATA_H_
#define PUSH_DATA_H_

#include <glm/glm.hpp>

// mesh push constants can be send many times in a single frame
// we can use this to send object model matrix many times since view and project remain same per frame
struct PushData {
    glm::mat4 objectModelMatrix;
    glm::mat4 normalMatrix;
};

#endif // PUSH_DATA_H_
