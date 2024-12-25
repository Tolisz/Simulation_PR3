#version 450 core

// --------------------------------------------
layout(quads, equal_spacing, ccw) in;
// --------------------------------------------

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

float B3( int i, float t)
{
    const vec4 bc = vec4( 1.0f, 3.0f, 3.0f, 1.0f );

    float pow1 = (t == 0.0f && i == 0) ? 1.0f : pow( t, float(i) );
    float pow2 = (t == 1.0f && i == 3) ? 1.0f : pow( 1.0f - t, float(3 - i) );
    return bc[i] * pow1 * pow2;
}

vec4 BezierPatch(float u, float v)
{
    vec4 pos = vec4( 0.0 );

    for ( int i = 0; i < 4; ++i ) {
        for ( int j = 0; j < 4; ++j ) {
            pos += B3( i, u ) * B3( j, v ) * gl_in[i * 4 + j].gl_Position;
        }
    }    
	
	pos.w = 1.0f;
    return pos;
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    vec4 pos = BezierPatch(u, v);

    gl_Position = projection * view * pos;
}