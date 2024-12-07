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

	bPoints = true;
	bShortSrpings = true;
	bLongSprings = false;
	bControlFrame = true;
	bCollisionFrame = true;
}