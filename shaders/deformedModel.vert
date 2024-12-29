#version 450 core

// =========================================
layout(location = 0) in vec3 iPosition;
// =========================================

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

layout(shared, binding = 0) buffer Points
{
    float P[192];
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

vec3 BezierCube(vec3 C)
{
    vec3 cubePoint = vec3(0.0f);
    for(int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                int idx = 3 * (16 * (3 - k) + 4 * (3 - j) + i);
                vec3 Point = vec3(P[idx], P[idx + 1], P[idx + 2]); 
                cubePoint += Point * B3(i, C.x) * B3(j, C.y) * B3(k, C.z);
            }
        }
    }

    return cubePoint;
}

void main()
{
    vec3 BoxPoint = vec3(toZeroOneBox * transformation * vec4(iPosition, 1.0f));
    vec3 CubePoint = BezierCube(BoxPoint);
    
	gl_Position = projection * view * vec4(CubePoint, 1.0f);
}