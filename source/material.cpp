#include "material.hpp"

material::material()
{
	ResetToDefault();
}

void material::ResetToDefault()
{
    ka = glm::vec3(0.2f);
    kd = glm::vec3(1.0f);
    ks = glm::vec3(0.5f);
    shininess = 128.0f;
}