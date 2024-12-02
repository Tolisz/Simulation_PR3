#version 450 core 

// =========================================
in FS_IN
{
    flat int pointIndex;
} i;

layout(location = 0) out vec4 oColor;
// =========================================

uniform int chosenPoint;

void main()
{
	oColor = vec4(chosenPoint == i.pointIndex ? 1.0f : 0.0f, 0.0f, 0.0f, 1.0f);
	// oColor = vec4(float(i.pointIndex) / 64.0f, 0.0f, 0.0f, 1.0f);
}