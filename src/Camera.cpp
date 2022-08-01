#include "Camera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>

// camera constructor
Camera::Camera(float fov, float ar)
    : fieldOfView(fov), aspectRatio(ar) {
    // update view matrix
    viewMatrix = glm::lookAt(position, position + front, up);

    // update projection matrix
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
}

// camera constructor
Camera::Camera(float fov, float ar, const glm::vec3& pos, const glm::vec3& up, const glm::vec3& front)
    : fieldOfView(fov), aspectRatio(ar), position(pos), up(up), front(front){
    // update view matrix
    viewMatrix = glm::lookAt(position, position + front, up);

    // update projection matrix
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
}

// update camera orientation and position in space
void Camera::update(glm::vec3 movement, glm::vec2 rotation, float deltaTime){
    // horizontal rotation
    front = glm::rotate(front, rotation.x * deltaTime/1000, up);

    // calculate camera right direction vector
    glm::vec3 right = glm::normalize(glm::cross(up, front));

    // vertical rotation
    front = glm::rotate(front, rotation.y * deltaTime/1000, right);

    up = glm::normalize(glm::cross(front, right));

    // move
    glm::vec3 deltaMove = movement * (deltaTime/1000);
    // camera faces positive x axis
    position += deltaMove.x * front;
    position += deltaMove.y * right;
    position += deltaMove.z * up;

    //position = glm::normalize(position);

    // update view matrix
    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::setAspectRatio(float ar) {
    aspectRatio = ar;

    // update projection matrix
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
}

float Camera::getAspectRatio() const{
    return aspectRatio;
}

void Camera::setFieldOfView(float fov) {
    fieldOfView = fov;

    // update projection matrix
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
}

float Camera::getFieldOfView() const{
    return fieldOfView;
}

