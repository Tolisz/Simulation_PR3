#include "camera.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext/matrix_transform.hpp>     // glm::lookAtRH
#include <glm/ext/matrix_clip_space.hpp>    // glm::perspective
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>

camera::camera()
    : m_worldPos(0.0f, 0.0f, 10.0f),
      m_front(0.0f, 0.0f, -1.0f),
      m_right(1.0f, 0.0f, 0.0f),
      m_up(0.0f, 1.0f, 0.0f),
      m_target(0.0f, 0.0f, 0.0f),
      m_worldUp(0.0f, 1.0f, 0.0f),

      m_nearPlane(0.01f),
      m_farPlane(1000.0f),
      m_fov(45.0f)
{}

camera::~camera()
{}

glm::mat4 camera::GetViewMatrix()
{
    return glm::lookAtRH(m_worldPos, m_worldPos + m_front, m_up);
}

glm::mat4 camera::GetProjectionMatrix(float aspect)
{
    return glm::perspectiveRH(glm::radians(m_fov), aspect, m_nearPlane, m_farPlane);
}

void camera::UpdatePosition(float posDelta)
{
    m_worldPos += m_front * posDelta;
}

void camera::UpdateRotation(float rotX, float rotY)
{
    // rotY -> rotate around m_right
    // rotX -> rotate around world Up
    glm::quat qY = glm::angleAxis(rotY, glm::normalize(m_right));
    glm::quat qX = glm::angleAxis(rotX, glm::normalize(m_worldUp));
    glm::quat conj_qY = glm::conjugate(qY);
    glm::quat conj_qX = glm::conjugate(qX);
    
    glm::quat v(0.0f, m_front); 
    glm::quat r = qX * qY * v * conj_qY * conj_qX;
    m_front = glm::vec3(r.x, r.y, r.z);

    v = glm::quat(0.0f, m_up);
    r = qX * qY * v * conj_qY * conj_qX;
    m_up = glm::vec3(r.x, r.y, r.z);

    v = glm::quat(0.0f, m_right);
    r = qX * v * conj_qX;
    m_right = glm::vec3(r.x, r.y, r.z);

    v = glm::quat(0.0f, m_worldPos);
    r = qX * qY * v * conj_qY * conj_qX;
    m_worldPos = glm::vec3(r.x, r.y, r.z);
}

