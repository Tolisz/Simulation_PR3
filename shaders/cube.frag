#version 450 core 

// =========================================
in FS_IN
{
    vec3 norm;
} i;

layout(location = 0) out vec4 oColor;
// =========================================

void main()
{
	oColor = vec4(i.norm, 1.0f);
}