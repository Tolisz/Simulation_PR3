#pragma once

#include <glm/vec4.hpp>

struct drawParameters
{
	glm::vec4 cPoints;
	glm::vec4 cShortSprings;
	glm::vec4 cLongSprings;
	glm::vec4 cControlFrame;
	glm::vec4 cCollisionFrame;

	bool bPoints;
	bool bShortSrpings;
	bool bLongSprings;
	bool bControlFrame;
	bool bCollisionFrame;

	drawParameters();

	void ResetToDefault();
};