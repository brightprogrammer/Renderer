/**
 * @file      Camera.hpp
 * @brief     Camera class declaration.
 * @date      Mon Dec  6 09:16:14 2021
 * @author    Siddharth Mishra
 */

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

class Camera{
public:
    /**
     * @brief Create camera with default data.
     * @param[in] fieldOfView of camera.
     * @param[in] aspectRatio of camera (width/height).
     */
    Camera(float fieldOfView, float aspectRatio);

    /**
     * @brief Create camera with given data.
     * @param[in] fieldOfView of camera.
     * @param[in] aspectRatio of camera (width/height).
     * @param[in] pos is the position of camera.
     * @param[in] up is the upper vector of camera.
     * @param[in] front is the front vector of camera.
     */
    Camera(float fieldOfView, float aspectRatio, const glm::vec3& pos, const glm::vec3& up, const glm::vec3& front);

    /**
     * @brief Update camera position.
     * @param[in] move vector. This will decide how far the camera will move in x,y and z coordinates.
     * @param[in] rotation (in degrees) of camera along yz and xz plane about camera up vector.
     * @param[in] deltaTime is time passed (in milliseconds) after previous call.
     */
    void update(glm::vec3 move, glm::vec2 rotation, float deltaTime);

    /// get view matrix of camera
    const glm::mat4& getViewMatrix() const{
        return viewMatrix;
    }

    /// get projection matrix of camera
    const glm::mat4& getProjectionMatrix() const{
        return projectionMatrix;
    }

    /// set camera aspect ratio
    void setAspectRatio(float aspectRatio);

    /// get aspect ratio
    float getAspectRatio() const;

    /// set field of view of camera
    void setFieldOfView(float fieldOfView);

    /// get field of view of camera
    float getFieldOfView() const;

    /// glm::vec3 alias for x, y and z axis
    static inline const glm::vec3 XAxis = glm::vec3(1, 0, 0), YAxis = glm::vec3(0, 1, 0), ZAxis = glm::vec3(0, 0, 1);

private:
    float aspectRatio;
    float fieldOfView = 70.f;
    float nearPlane = 0.1f;
    float farPlane = 200.f;

    glm::vec3 position = glm::vec3(0); // origin
    glm::vec3 up = glm::vec3(0, 1, 0); // y-axis
    glm::vec3 front = glm::vec3(0, 0, 1); // z-axis

    // camera view and projection matrices
    glm::mat4 viewMatrix, projectionMatrix;
};

#endif//CAMERA_HPP
