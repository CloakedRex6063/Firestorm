#version 460
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inUV;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inPos;
layout(location = 4) in vec3 inCamPos;
layout(location = 5) in flat uint inLightCount;

layout(location = 0) out vec4 outFragColor;

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
    int occlusionTextureIndex;
    float ao;
    int emissiveTextureIndex;
    vec3 emissiveFactor;
    int alphaMode;
    bool doubleSided;
    float alphaCutoff;
    float ior;
};

struct Texture
{
    int samplerIndex;
    int imageIndex;
};

layout(buffer_reference, std430) readonly buffer VertexBuffer { Vertex vertices[]; };

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

struct Light
{
    vec3 position;
    int type;
    vec3 color;
    float intensity;
    vec3 direction;
};

layout(binding = 1) uniform sampler2D samplers[];

layout(binding = 2) readonly buffer LightBuffer { Light lights[]; };

const float PI = 3.14159265359;

vec3 FresnelShlick(float cosTheta, vec3 F0) { return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0, 1), 5); }

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return a2 / denom;
}

float GeometryShclickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = r * r / 8.0;

    float denom = NdotV * (1.0 - k) + k;
    return NdotV / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometryShclickGGX(NdotL, roughness);
    float ggx2 = GeometryShclickGGX(NdotV, roughness);

    return ggx1 * ggx2;
}

void main()
{
    Material material = pushConstant.materialBuffer.materials[pushConstant.materialIndex];
    Texture tex = pushConstant.texBuffer.textures[material.baseTextureIndex];

    vec3 albedo = pow(texture(samplers[tex.imageIndex], inUV).rgb, vec3(2.2));
    vec4 baseColor = material.baseTextureIndex != -1 ? vec4(albedo, 1.0) : material.baseColorFactor;

    vec3 N = normalize(inNormal);
    vec3 V = normalize(inCamPos - inPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, vec3(baseColor), material.metallicFactor);

    vec3 Lo = vec3(0);
    for (int i = 0; i < inLightCount; i++)
    {
        Light light = lights[i];
        vec3 lightToModel = light.position - inPos;
        vec3 L = normalize(lightToModel);
        vec3 H = normalize(V + L);
        float distance = length(lightToModel);
        float attenutation = 1 / distance * distance;
        vec3 radiance = light.color * attenutation;

        float NDF = DistributionGGX(N, H, material.roughnessFactor);
        float G = GeometrySmith(N, V, L, material.roughnessFactor);
        vec3 F = FresnelShlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - material.metallicFactor;

        vec3 num = NDF * G * F;
        float denom = 4 * max(dot(N, V), 0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = num / denom;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * vec3(baseColor) / PI + specular) * radiance * NdotL;
    }
    vec3 ambient = vec3(0.03) * vec3(baseColor) * material.ao;
    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    outFragColor = vec4(color, 1.0) * inColor;
}
