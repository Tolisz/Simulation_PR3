#version 450 core

// =========================================
layout(location = 0) in vec3 iPosition;

out FS_IN
{
    flat int pointIndex;
} o;
// =========================================

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

void main()
{
    o.pointIndex = gl_VertexID;
	gl_Position = projection * view * vec4(iPosition, 1.0f);
}