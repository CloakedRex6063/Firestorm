#version 460
#extension GL_EXT_buffer_reference : require 
#extension GL_EXT_nonuniform_qualifier : require 

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inPos;

layout (location = 0) out vec4 outFragColor;

struct Vertex
{
    vec3 position;
    float uvX;
    vec3 normal;
    float uvY;
    vec4 color;
};

struct Material
{
    vec4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
    int baseTextureIndex;
    int roughnessTextureIndex;
};

layout(buffer_reference, std430) readonly buffer VertexBuffer
{
    Vertex vertices[];
};

layout(buffer_reference, std430) readonly buffer MaterialBuffer
{
    Material materials[];
};

layout(push_constant) uniform Constant
{
    mat4 renderMatrix;
    VertexBuffer vertexBuffer;
    MaterialBuffer materialBuffer;
    uint materialIndex;
    uint lightCount;
} pushConstant;

struct Light
{
    vec3 position; 
    int type;
    vec3 color;
    float intensity;
    vec3 direction; 
};

layout(binding = 1) uniform sampler2D samplers[];

layout(binding = 2) readonly buffer LightBuffer
{
    Light lights[];
};

void main()
{
    vec3 totalLightColor = vec3(0);
    for (int i = 0; i < pushConstant.lightCount; i++) 
    {
        Light light = lights[i];

        vec3 lightDir;
        float attenuation = 1.0f;
        switch(light.type)
        {
            case 0:
                lightDir = normalize(light.position - inPos);
                float distance    = length(light.position - inPos);
                attenuation = light.intensity / (distance * distance);
                break;
            case 1:
                lightDir = normalize(-light.direction);
                
                break;
        }
        float diff = max(dot(inNormal, lightDir), 0.0);
        vec3 diffuse = diff * light.color * attenuation;
        totalLightColor += diffuse;
    }
    Material material = pushConstant.materialBuffer.materials[pushConstant.materialIndex];
    vec4 baseColor = material.baseTextureIndex != -1
    ? texture(samplers[material.baseTextureIndex], inUV)
    : material.baseColorFactor;
    outFragColor = baseColor * inColor * vec4(totalLightColor, 1);
}
