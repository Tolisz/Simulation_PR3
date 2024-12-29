#version 450 core

// =========================================
layout(location = 0) in vec3 iPosition;
// =========================================

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

uniform mat4 toZeroOneBox;
uniform mat4 transformation;

float B3( int i, float t)
{
    const vec4 bc = vec4( 1.0f, 3.0f, 3.0f, 1.0f );

    float pow1 = (t == 0.0f && i == 0) ? 1.0f : pow( t, float(i) );
    float pow2 = (t == 1.0f && i == 3) ? 1.0f : pow( 1.0f - t, float(3 - i) );
    return bc[i] * pow1 * pow2;
}



void main()
{
    vec3 BoxPoint = vec3(toZeroOneBox * transformation * vec4(iPosition, 1.0f));

	gl_Position = projection * view * vec4(BoxPoint, 1.0f);
}