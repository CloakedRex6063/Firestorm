#version 460
#extension GL_EXT_buffer_reference : require 
#extension GL_EXT_nonuniform_qualifier : require 

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inUV;

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
} pushConstants;

struct Light
{
    vec4 position;  // xyz for position, w for type (point, spot, etc.)
    vec4 color;     // rgb for color, w for intensity
    vec4 direction; // xyz for direction, w ignored (for now)
};

layout(binding = 1) uniform sampler2D samplers[];

layout(binding = 2) readonly buffer LightBuffer
{
    Light lights[];
};

void main()
{
    Material material = pushConstants.materialBuffer.materials[pushConstants.materialIndex];
    vec4 baseColor = material.baseTextureIndex != -1
    ? texture(samplers[material.baseTextureIndex], inUV)
    : material.baseColorFactor;
    outFragColor = baseColor * lights[0].color;
}
