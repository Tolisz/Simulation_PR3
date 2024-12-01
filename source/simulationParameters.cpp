#include "simulationParameters.hpp"

simulationParameters::simulationParameters()
{
	m.fill(1.0f);

	a = 1.0f;
	c1 = 1.0f;
	k = 1.0f;
	c2 = 1.0f;

	dt = 0.01f;
}