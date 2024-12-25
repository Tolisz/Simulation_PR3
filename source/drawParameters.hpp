#pragma once

#include <glm/vec4.hpp>

struct drawParameters
{
	glm::vec4 cPoints;
	glm::vec4 cShortSprings;
	glm::vec4 cLongSprings;
	glm::vec4 cControlFrame;
	glm::vec4 cCollisionFrame;
	glm::vec4 cJelly;

	bool bPoints;
	bool bShortSrpings;
	bool bLongSprings;
	bool bControlFrame;
	bool bCollisionFrame;
	bool bJelly;
	bool bModel;

	float mJellyTessellationLevel;

	drawParameters();

	void ResetToDefault();
};