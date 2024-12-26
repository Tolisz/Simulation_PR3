#pragma once

#include <glm/vec3.hpp>

struct material
{
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;
    float shininess;

	material();

	void ResetToDefault();
};

