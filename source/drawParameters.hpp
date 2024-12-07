#pragma once

#include <glm/vec4.hpp>

struct drawParameters
{
	glm::vec4 cPoints;
	glm::vec4 cShortSpringsColor;
	glm::vec4 cLongSpringsColor;

	bool bPoints;
	bool bShortSrpings;
	bool bLongSprings;

	drawParameters();

	void ResetToDefault();
};