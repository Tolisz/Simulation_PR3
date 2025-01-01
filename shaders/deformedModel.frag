#version 450 core

// =========================================
layout(location = 0) out vec4 oColor;

in FS_IN
{
    vec2 texCoords;
    vec3 normal;
} i;
// =========================================

uniform sampler2D texture_diffuse0;

void main()
{
	vec3 texColor = texture(texture_diffuse0, i.texCoords).xyz;
	oColor = vec4(i.normal, 1.0f);
}