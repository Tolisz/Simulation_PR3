#version 450 core

// =========================================
layout(location = 0) in vec3 iPosition;
// =========================================

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

uniform mat4 transformation;

void main()
{
	gl_Position = projection * view * transformation * vec4(iPosition, 1.0f);
}