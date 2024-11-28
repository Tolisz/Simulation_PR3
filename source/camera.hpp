#pragma once

#include <glm/mat4x4.hpp>

class camera
{

public:

    camera();
    ~camera() = default;

    camera(const camera&) = delete;
    camera(camera&&) = delete;
    camera& operator=(const camera&) = delete;
    camera& operator=(camera&&) = delete;

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float aspect);

    void Rotate(float deltaX, float deltaY);
    void Zoom(float factor);
    void Move(float deltaX, float deltaY);

private:

    glm::vec3 m_focusPos;
    glm::vec3 m_worldPos;
    glm::vec3 m_front;
    glm::vec3 m_right;
    glm::vec3 m_up;
    glm::vec3 m_target;
    glm::vec3 m_worldUp;

private: 
    float m_rotationSpeed = 0.01f;
    float m_moveSpeed = 0.001f;
    float m_zoomSpeed = 0.25f;

private:
    float m_nearPlane;
    float m_farPlane;
    float m_fov;
};