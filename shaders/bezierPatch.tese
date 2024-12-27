#version 450 core

// =========================================
layout(quads, equal_spacing, ccw) in;

out FS_IN
{
    vec3 worldPos;
    vec2 texCoords;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
} o;
// =========================================

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

float B2(int i, float t)
{
    const vec3 bc = vec3( 1.0f, 2.0f, 1.0f );

    float pow1 = (t == 0.0f && i == 0) ? 1.0f : pow( t, float(i) );
    float pow2 = (t == 1.0f && i == 2) ? 1.0f : pow( 1.0f - t, float(2 - i) );
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

vec3 BezierPatchNormal(float u, float v, out vec3 T, out vec3 B)
{
    vec3 t1 = vec3(0.0f);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            t1 += 3 * (gl_in[(i + 1) * 4 + j].gl_Position.xyz - gl_in[i * 4 + j].gl_Position.xyz) * B2(i, u) * B3(j, v);
        }
    }

    T = normalize(t1);

    vec3 t2 = vec3(0.0f);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) {
            t2 += 3 * (gl_in[i * 4 + (j + 1)].gl_Position.xyz - gl_in[i * 4 + j].gl_Position.xyz) * B3(i, u) * B2(j, v);
        }
    }

    B = normalize(t2);

    return cross(T, B);
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    o.texCoords = vec2(u, v);

    vec4 pos = BezierPatch(u, v);
    vec3 norm = BezierPatchNormal(u, v, o.tangent, o.bitangent);

    o.worldPos = pos.xyz;
    o.normal = norm;
    
    gl_Position = projection * view * pos;
}