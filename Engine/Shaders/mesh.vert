#version  460
#extension GL_EXT_buffer_reference : require 

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

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outUV;

void main() 
{
    Vertex v = pushConstants.vertexBuffer.vertices[gl_VertexIndex];
    
    gl_Position = pushConstants.renderMatrix * vec4(v.position, 1.0);
    outUV = vec2(v.uvX, v.uvY);
    outColor = v.color;
}