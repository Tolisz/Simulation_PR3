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
	cCollisionFrame = glm::vec4(0.467f, 0.467f, 0.467f, 0.788f);
	cJelly = glm::vec4(0.607f, 0.39f, 0.768f, 1.0f);

	bPoints = false;
	bShortSrpings = false;
	bLongSprings = false;
	bControlFrame = true;
	bCollisionFrame = true;
	bJelly = true;

	mJellyTessellationLevel = 10;
	mPointSize = 4;
}