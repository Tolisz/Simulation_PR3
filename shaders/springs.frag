#version 450 core 

// =========================================
layout(location = 0) out vec4 oColor;
// =========================================

uniform vec4 springColor;

void main()
{
	oColor = springColor;
	// oColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}