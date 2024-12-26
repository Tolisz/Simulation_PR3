#pragma once 

#include <glm/vec4.hpp>

struct light
{
    glm::vec4 m_position;
    glm::vec4 m_diffuseColor;
    glm::vec4 m_specularColor;	

	light();

	void ResetToDefault();
};
