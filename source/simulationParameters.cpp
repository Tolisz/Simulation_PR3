#include "simulationParameters.hpp"

simulationParameters::simulationParameters()
{
	ResetToDefault();
}

void simulationParameters::ResetToDefault()
{
	m.fill(1.0f);

	a = 1.0f;
	c1 = 20.0f;
	k = 1.0f;
	c2 = 1.0f;

	dt = 0.01f;

	bControlFrame = true;
	bCollisionFrame = true;
}