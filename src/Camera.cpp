#include "Camera.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "KeyboardState.hpp"
#include "MouseState.hpp"

// camera constructor
Camera::Camera(float fov, float ar)
    : fieldOfView(fov), aspectRatio(ar) {
    // update view matrix
    viewMatrix = glm::lookAt(position, position + front, up);

    // update projection matrix
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
    projectionMatrix[1][1] *= -1; // flip Y axis
}

// camera constructor
Camera::Camera(float fov, float ar, const glm::vec3& pos, const glm::vec3& up, const glm::vec3& front)
    : fieldOfView(fov), aspectRatio(ar), position(pos), up(up), front(front){
    // update view matrix
    viewMatrix = glm::lookAt(position, position + front, up);

    // update projection matrix
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
    projectionMatrix[1][1] *= -1; // flip Y axis
}

// update camera orientation and position in space
void Camera::update(const glm::vec3& move, const glm::vec2& rotation, float deltaTime){
    // convert delta time to seconds
    deltaTime = deltaTime / 1e3f;

    // horizontal rotation
    front = glm::rotate(front, rotation.x * deltaTime, up);

    // calculate camera right direction vector
    glm::vec3 right = glm::normalize(glm::cross(up, front));

    // vertical rotation
    front = glm::rotate(front, rotation.y * deltaTime, right);

    up = glm::normalize(glm::cross(front, right));

    // move
    glm::vec3 deltaMove = move * deltaTime; // convert to seconds
    // camera faces positive x axis
    position += deltaMove.x * front;
    position += deltaMove.y * right;
    position += deltaMove.z * up;

    //position = glm::normalize(position);

    // update view matrix
    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::updateAspectRatio(SDL_Window* window){
    int windowWidth, windowHeight;

    // get window size
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);

    // update camera aspect ratio
    aspectRatio = static_cast<float>(windowWidth)/windowHeight;

    // update projection matrix
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
    projectionMatrix[1][1] *= -1; // flip Y axis
}

void Camera::setAspectRatio(float ar) {
    aspectRatio = ar;

    // update projection matrix
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
    projectionMatrix[1][1] *= -1; // flip Y axis
}

float Camera::getAspectRatio() const{
    return aspectRatio;
}

void Camera::setFieldOfView(float fov) {
    fieldOfView = fov;

    // update projection matrix
    projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
    projectionMatrix[1][1] *= -1; // flip Y axis
}

float Camera::getFieldOfView() const{
    return fieldOfView;
}

