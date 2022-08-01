#ifndef MATH_H_
#define MATH_H_

#include <glm/glm.hpp>

#define PI 3.14159265359

/**
 * @brief Convert point in spherical coordinates to point in cartesian coordinates.
 *
 * @param radius Radial position of point.
 * @param theta Horizontal angular position of point.
 * @param phi Vertical angular position of point.
 * @return glm::vec3 Point in cartesian coordinates.
 * */
glm::vec3 sphericalToCartesian(float radius, float theta, float phi);

#endif // MATH_H_
