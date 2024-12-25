#include "drawParameters.hpp"

drawParameters::drawParameters()
{
	ResetToDefault();
}

void drawParameters::ResetToDefault()
{
	cPoints = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cShortSprings = glm::vec4(0.54f, 1.0f, 0.0f, 1.0f);
	cLongSprings = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	cControlFrame = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	cCollisionFrame = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	cJelly = glm::vec4(0.607f, 0.39f, 0.768f, 1.0f);

	bPoints = true;
	bShortSrpings = true;
	bLongSprings = false;
	bControlFrame = true;
	bCollisionFrame = true;
	bJelly = true;

	mJellyTessellationLevel = 10;
}