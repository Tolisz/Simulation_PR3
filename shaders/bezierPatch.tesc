#version 450 core

layout(vertices = 16) out;

uniform float tessellationLevel;

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	gl_TessLevelOuter[0] = tessellationLevel;
	gl_TessLevelOuter[1] = tessellationLevel;
	gl_TessLevelOuter[2] = tessellationLevel;
	gl_TessLevelOuter[3] = tessellationLevel;
        
	gl_TessLevelInner[0] = tessellationLevel;
	gl_TessLevelInner[1] = tessellationLevel;
}