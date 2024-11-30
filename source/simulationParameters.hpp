#pragma once 

struct simulationParameters
{
	float m[64];	// 64 mass	
	float c1;		// spring constant of springs inside Bezier Cube 
	float k;		// damping
	float c2;		// spring constant of springs from Cube to Frame
	
	float dt;		// integration step

	simulationParameters();
};
