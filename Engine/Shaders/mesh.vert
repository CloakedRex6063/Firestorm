#version 460
#extension GL_EXT_buffer_reference : require

layout(location = 1) out vec2 outUV;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec3 outPos;
layout(location = 4) out vec3 outCamPos;
layout(location = 5) out flat uint outLightCount;

struct Vertex
{
    vec3 position;
    float uvX;
    vec3 normal;
    float uvY;
};

layout(buffer_reference, std430) readonly buffer VertexBuffer { Vertex vertices[]; };

struct Material
{
    vec4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
    int baseTextureIndex;
    int roughnessTextureIndex;
};

struct Texture
{
    int samplerIndex;
    int imageIndex;
};

layout(buffer_reference, std430) readonly buffer MaterialBuffer { Material materials[]; };

layout(buffer_reference, std430) readonly buffer TextureBuffer { Texture textures[]; };

layout(push_constant) uniform Constant
{
    mat4 model;
    VertexBuffer vertexBuffer;
    MaterialBuffer materialBuffer;
    TextureBuffer texBuffer;
    int materialIndex;
}
pushConstant;

layout(binding = 1) uniform UBO
{
    vec3 camPos;
    uint lightCount;
    mat4 view;
    mat4 projection;
};

void main()
{
    Vertex v = pushConstant.vertexBuffer.vertices[gl_VertexIndex];

    gl_Position = projection * view * pushConstant.model * vec4(v.position, 1.0);
    outUV = vec2(v.uvX, v.uvY);
    outPos = vec3(pushConstant.model * vec4(v.position, 1.0));
    outNormal = normalize((pushConstant.model * vec4(v.normal, 0.0)).xyz);
    outCamPos = vec3(camPos);
    outLightCount = lightCount;

}