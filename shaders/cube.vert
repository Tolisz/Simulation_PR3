#version 450 core

// =========================================
layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;

out FS_IN
{
    vec3 norm;
} o;
// =========================================

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

uniform mat4 model; 

void main()
{
	o.norm = mat3(transpose(inverse(model))) * iNormal;
    o.norm = -iNormal;
	gl_Position = projection * view * model * vec4(iPosition, 1.0f);
}