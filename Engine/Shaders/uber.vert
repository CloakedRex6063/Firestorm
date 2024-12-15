#version 460
#extension GL_EXT_buffer_reference : require

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outPos;
layout(location = 3) out mat3 outTBN;
layout(location = 6) out vec3 outCamPos;
layout(location = 7) out flat uint outLightCount;

struct Vertex
{
    vec3 position;
    float uvX;
    vec3 normal;
    float uvY;
    vec3 tangent;
    float padding0;
    vec3 bitangent;
    float padding1;
};

layout(buffer_reference, std430) readonly buffer VertexBuffer { Vertex vertices[]; };

struct Material
{
    vec4 baseColorFactor;

    int baseTextureIndex;
    int roughnessTextureIndex;
    int occlusionTextureIndex;
    int emissiveTextureIndex;

    vec3 emissiveFactor;
    float metallicFactor;

    float roughnessFactor;
    float ao;
    float alphaCutoff;
    float ior;

    int normalTextureIndex;
    int padding0;
    int padding1;
    int padding2;
};

struct Texture
{
    int imageIndex;
};

layout(buffer_reference, std430) readonly buffer MaterialBuffer { Material materials[]; };

layout(buffer_reference, std430) readonly buffer TextureBuffer { Texture textures[]; };

layout(push_constant) uniform Constant
{
    mat4 model;
    VertexBuffer vertexBuffer;
    MaterialBuffer materialBuffer;
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
    outNormal = normalize(vec3(pushConstant.model * vec4(v.normal, 0.0)));
    outCamPos = vec3(camPos);
    outLightCount = lightCount;

    vec3 tangent = normalize(vec3(pushConstant.model * vec4(v.tangent,   0.0)));
    vec3 bitangent = normalize(vec3(pushConstant.model * vec4(v.bitangent, 0.0)));
    outTBN = mat3(tangent, bitangent, outNormal);
}