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
	k = 3.0f;
	c2 = 30.0f;
	mu = 1.0f;	// 1.0f - elastic collision
	dt = 0.01f;

	bControlFrame = true;
	bCollisionFrame = true;
}