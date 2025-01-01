#version 450 core

// =========================================
layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec2 iTexCoords;
layout(location = 2) in vec3 iNormal;


out FS_IN
{
    vec2 texCoords;
    vec3 normal;
} o;
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

float B2(int i, float t)
{
    const vec3 bc = vec3( 1.0f, 2.0f, 1.0f );

    float pow1 = (t == 0.0f && i == 0) ? 1.0f : pow( t, float(i) );
    float pow2 = (t == 1.0f && i == 2) ? 1.0f : pow( 1.0f - t, float(2 - i) );
    return bc[i] * pow1 * pow2;    
}

vec3 GetPoint(int i, int j, int k)
{
    int idx = 3 * (16 * (3 - k) + 4 * (3 - j) + i);
    return vec3(P[idx], P[idx + 1], P[idx + 2]);
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
                vec3 Point = GetPoint(i, j, k);
                cubePoint += Point * B3(i, C.x) * B3(j, C.y) * B3(k, C.z);
            }
        }
    }

    return cubePoint;
}

mat3 adjoint(mat3 m) {
    return mat3(
        m[1][1] * m[2][2] - m[1][2] * m[2][1], // Cofactor of m[0][0]
        m[0][2] * m[2][1] - m[0][1] * m[2][2], // Cofactor of m[0][1]
        m[0][1] * m[1][2] - m[0][2] * m[1][1], // Cofactor of m[0][2]
        
        m[1][2] * m[2][0] - m[1][0] * m[2][2], // Cofactor of m[1][0]
        m[0][0] * m[2][2] - m[0][2] * m[2][0], // Cofactor of m[1][1]
        m[0][2] * m[1][0] - m[0][0] * m[1][2], // Cofactor of m[1][2]
        
        m[1][0] * m[2][1] - m[1][1] * m[2][0], // Cofactor of m[2][0]
        m[0][1] * m[2][0] - m[0][0] * m[2][1], // Cofactor of m[2][1]
        m[0][0] * m[1][1] - m[0][1] * m[1][0]  // Cofactor of m[2][2]
    );
}

vec3 ComputeNormal(vec3 C, vec3 N, mat3 modelMatrix) 
{
    vec3 F_u = vec3(0.0f);
    vec3 F_v = vec3(0.0f);
    vec3 F_w = vec3(0.0f);

    // F_u
    for(int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
                vec3 PP = GetPoint(i + 1, j, k);
                vec3 PN = GetPoint(i, j, k);
                F_u += (PP - PN) * B2(i, C.x) * B3(j, C.y) * B3(k, C.z);
            }
        }
    } 

    // F_v
    for(int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                vec3 PP = GetPoint(i, j + 1, k);
                vec3 PN = GetPoint(i, j, k);
                F_v += (PP - PN) * B3(i, C.x) * B2(j, C.y) * B3(k, C.z);
            }
        }
    } 
    
    // F_w
    for(int i = 0; i < 4; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                vec3 PP = GetPoint(i, j, k + 1);
                vec3 PN = GetPoint(i, j, k);
                F_w += (PP - PN) * B3(i, C.x) * B3(j, C.y) * B2(k, C.z);
            }
        }
    }
    
    mat3 J_kreska = mat3(F_u, F_v, F_w);
    vec3 newN = transpose(inverse(modelMatrix) * adjoint(J_kreska)) * N;
    return normalize(newN);
}

void main()
{
    mat4 Model = toZeroOneBox * transformation;
    
    vec3 BoxPoint = vec3(Model * vec4(iPosition, 1.0f));
    vec3 BoxNormal = mat3(transpose(inverse(Model))) * iNormal; 
    vec3 DefformedPoint = BezierCube(BoxPoint);
    vec3 DefformedNormal = vec3(0.0f);

	gl_Position = projection * view * vec4(DefformedPoint, 1.0f);
    o.texCoords = iTexCoords;
    o.normal = ComputeNormal(BoxPoint, iNormal, mat3(Model));  
}