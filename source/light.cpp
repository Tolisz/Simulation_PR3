#include "light.hpp"

light::light()
{
	ResetToDefault();
}

void light::ResetToDefault()
{
	m_position = glm::vec4(0.0f);
	m_diffuseColor = glm::vec4(1.0f);
	m_specularColor = glm::vec4(1.0f);
}

