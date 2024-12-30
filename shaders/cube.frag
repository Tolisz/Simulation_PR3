#version 450 core 

// =========================================
in FS_IN
{
    vec3 norm;
} i;

layout(location = 0) out vec4 oColor;
// =========================================

uniform vec4 cubeColor;

void main()
{
	oColor = cubeColor;
}