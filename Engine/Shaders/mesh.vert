#version 460
#extension GL_EXT_buffer_reference : require 

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outUV;

struct Vertex
{
    vec3 position;
    float uvX;
    vec3 normal;
    float uvY;
    vec3 color;
};

layout(buffer_reference, std430) readonly buffer VertexBuffer
{
    Vertex vertices[];
};

layout(push_constant) uniform PushConstants
{
    mat4 renderMatrix;
    VertexBuffer vertexBuffer;
} pushConstants;

void main() 
{
    Vertex v = pushConstants.vertexBuffer.vertices[gl_VertexIndex];
    gl_Position = pushConstants.renderMatrix * vec4(v.position, 1.f);
    outColor = v.color.xyz;
    outUV.x = v.uvX;
    outUV.y = v.uvY;
}