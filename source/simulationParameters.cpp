#include "simulationParameters.hpp"

#include <algorithm>

simulationParameters::simulationParameters()
{
	std::fill_n(m, 64, 1.0f);
	
	c1 = 1.0f;
	k = 1.0f;
	c2 = 1.0f;
}