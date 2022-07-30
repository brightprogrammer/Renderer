#ifndef MESHPUSHCONSTANTS_H_
#define MESHPUSHCONSTANTS_H_

#include <glm/glm.hpp>

// mesh push constants can be send many times in a single frame
// we can use this to send object model matrix many times since view and project remain same per frame
struct MeshPushConstants {
    glm::mat4 objectModelMatrix;
};

#endif // MESHPUSHCONSTANTS_H_
