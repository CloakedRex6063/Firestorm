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

layout(buffer_reference, std430) readonly buffer VertexBuffer
{
    Vertex vertices[];
};

layout(push_constant) uniform constants
{
    mat4 renderMatrix;
    VertexBuffer vertexBuffer;
    int textureIndex;
} pushConstants;

layout(binding = 1) uniform sampler2D Sampler2D[];

void main()
{
    outFragColor = texture(Sampler2D[pushConstants.textureIndex], inUV);
}