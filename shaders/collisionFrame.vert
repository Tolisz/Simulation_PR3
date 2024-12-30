#version 450 core

// =========================================
layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec3 iNormal;

out FS_IN
{
	vec3 worldPos;
    vec3 normal;
} o;
// =========================================

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

uniform mat4 model; 

void main()
{
	o.normal = mat3(transpose(inverse(model))) * iNormal;
    o.normal = -iNormal;

	vec4 worldPos = model * vec4(iPosition, 1.0f);
	o.worldPos = worldPos.xyz;

	gl_Position = projection * view * worldPos;
}