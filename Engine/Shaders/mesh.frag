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

layout(push_constant) uniform constants
{
    mat4 renderMatrix;
    VertexBuffer vertexBuffer;
    MaterialBuffer materialBuffer;
    int materialIndex;
} pushConstants;

layout(binding = 1) uniform sampler2D Sampler2D[];

void main()
{
    Material material = pushConstants.materialBuffer.materials[pushConstants.materialIndex];
    vec4 baseColor = material.baseColorFactor * texture(Sampler2D[material.baseTextureIndex], inUV);
    vec4 metallicRoughness = texture(Sampler2D[material.roughnessTextureIndex], inUV);
    float metallic = metallicRoughness.b * material.metallicFactor;
    float roughness = metallicRoughness.g * material.roughnessFactor;

    // Output the final fragment color
    outFragColor = baseColor;
}
