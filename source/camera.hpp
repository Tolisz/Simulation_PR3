#pragma once

#include <glm/mat4x4.hpp>

class camera
{

    
public:
    glm::vec3 m_worldPos;
    glm::vec3 m_front;
    glm::vec3 m_right;
    glm::vec3 m_up;
    glm::vec3 m_target;
    glm::vec3 m_worldUp;

public:
    float m_nearPlane;
    float m_farPlane;
    float m_fov;
    float m_cameraSpeed = 0.01f; 

public:

    // *=*=*=*=*=*=*=*=*=*=
    //   Object Creation
    // *=*=*=*=*=*=*=*=*=*=

    camera();
    ~camera();

    camera(const camera&) = delete;
    camera(camera&&) = delete;
    camera& operator=(const camera&) = delete;
    camera& operator=(camera&&) = delete;

    // *=*=*=*=*=*=*=*=*=*=
    //      Methods
    // *=*=*=*=*=*=*=*=*=*=

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix(float aspect);

    void UpdatePosition(float posDelta);  
    void UpdateRotation(float rotX, float rotY);
};