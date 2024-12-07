#include "drawParameters.hpp"

drawParameters::drawParameters()
{
	ResetToDefault();
}

void drawParameters::ResetToDefault()
{
	cPoints = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cShortSpringsColor = glm::vec4(0.54f, 1.0f, 0.0f, 1.0f);
	cLongSpringsColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	
	bPoints = true;
	bShortSrpings = true;
	bLongSprings = false;
}