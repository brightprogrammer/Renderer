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
#include <SDL2/SDL.h>
#include <iostream>

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

    ~Camera(){
        std::cout << position.x << " " << position.y << "  " << position.z << std::endl;
        std::cout << up.x << " " << up.y << "  " << up.z << std::endl;
        std::cout << front.x << " " << front.y << "  " << front.z << std::endl;
    }

    /**
     * @brief Update camera position and orientation.
     * This must be called after calculation for motion and rotation events are done
     * per frame. That is only once in a while loop.
     *
     * @param[in] move 3D vector describing motion in x, y and z direction.
     * @param[in] rotation 2D vector describing vertical and horizontal rotation.
     * @param[in] deltaTime is time passed (in milliseconds) after previous call.
     */
    void update(const glm::vec3& move, const glm::vec2& rotation, float deltaTime);

    /**
     * @brief Update camera aspect ratio based on window size.
     *
     * @param window Window that is using this camera.
     * */
    void updateAspectRatio(SDL_Window* window);

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

    /// get camera position
    inline const glm::vec3& getPosition() const { return position; }

    /// get camera up direction
    inline const glm::vec3& getUpDirection() const { return up; }

    /// get camera front direction
    inline const glm::vec3& getFrontDirection() const { return front; }

    /// camera linear speed
    glm::vec3 linearSpeed = glm::vec3(10.f);

    /// camera angular speed
    glm::vec2 angularSpeed = glm::vec2(0.20f);
private:
    float fieldOfView = 70.f;
    float aspectRatio = 16.f/9;
    float nearPlane = 0.1f;
    float farPlane = 300.f;

    glm::vec3 position = glm::vec3(0); // origin
    glm::vec3 up = YAxis; // y-axis
    glm::vec3 front = ZAxis; // z-axis

    // camera view and projection matrices
    glm::mat4 viewMatrix, projectionMatrix;
};

#endif//CAMERA_HPP
