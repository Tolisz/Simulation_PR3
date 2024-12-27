#version 450 core 

#define MAX_NUM_OF_LIGHTS 5

// =========================================
layout(location = 0) out vec4 oColor;

in FS_IN
{
    vec3 worldPos;
    vec2 texCoords;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
} i;
// =========================================

struct Light
{
    vec4 position;
    vec4 diffuseColor;
    vec4 specularColor;
};

struct Material
{
    vec3 ka, kd, ks;
    float shininess;
};

layout(std140, binding = 1) uniform LightsBlock {
    vec4 ambientColor;
    Light light[MAX_NUM_OF_LIGHTS];
};
uniform int numberOfLights;

uniform Material material;
uniform vec3 cameraPos;

uniform vec4 patchColor;

vec3 Phong(vec3 worldPos, vec3 norm, vec3 view, vec3 surfaceColor)
{
    vec3 N = normalize(norm);
    vec3 V = normalize(view);

    // ambient 
    vec3 color = material.ka * ambientColor.rgb;

    for (int i = 0; i < min(numberOfLights, MAX_NUM_OF_LIGHTS); i++) {
        // diffuse 
        vec3 L = normalize(light[i].position.xyz - worldPos);
        color += material.kd * light[i].diffuseColor.rgb * surfaceColor * max(dot(N, L), 0.0f);

        // specular
        vec3 R = reflect(-L, N);
        color += material.ks * light[i].specularColor.rgb * pow(max(dot(R, V), 0.0f), material.shininess);
    }

    return color;
}

void main()
{
    vec3 N = normalize(i.normal);
	vec3 V = normalize(cameraPos - i.worldPos);

	vec3 col = Phong(i.worldPos, N, V, patchColor.rgb);

	oColor = vec4(col, 1.0f);
}