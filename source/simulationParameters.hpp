#pragma once 

#include <array>

struct simulationParameters
{
	std::array<float, 64> m;	// 64 mass	

	float a; 		// Cube's edge size
	float c1;		// spring constant of springs inside Bezier Cube 
	float k;		// viscous friction
	float c2;		// spring constant of springs from Cube to Frame
	float mu; 		// coefficient of restitution
	float dt;		// integration step

	bool bControlFrame;
	bool bCollisionFrame;

	simulationParameters();

	void ResetToDefault();
};
