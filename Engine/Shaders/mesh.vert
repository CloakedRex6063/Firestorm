#version  460
#extension GL_EXT_buffer_reference : require 

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outPos;

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

layout(push_constant) uniform Constant
{
    mat4 model;
    VertexBuffer vertexBuffer;
} pushConstant;

layout(binding = 0) uniform UBO
{
    mat4 view;
    mat4 projection;
};

void main() 
{
    Vertex v = pushConstant.vertexBuffer.vertices[gl_VertexIndex];
    
    gl_Position = projection * view * pushConstant.model * vec4(v.position, 1.0);
    outUV = vec2(v.uvX, v.uvY);
    outPos = vec3(pushConstant.model * vec4(v.position, 1.0));
    outColor = v.color;
    // TODO: calculate normals on cpu as inverse is an expensive operation
    outNormal = mat3(transpose(inverse(pushConstant.model))) * v.normal;
}