#version 460
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inPos;
layout(location = 3) in mat3 inTBN;
layout(location = 6) in vec3 inCamPos;
layout(location = 7) in flat uint inLightCount;

layout(location = 0) out vec4 outFragColor;

struct Vertex
{
    vec3 position;
    float uvX;
    vec3 normal;
    float uvY;
};

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

layout(binding = 0) uniform sampler2D samplers[];

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

vec3 GetNormalFromMap(Texture tex)
{
    vec3 tangentNormal = texture(samplers[tex.imageIndex], inUV).xyz * 2.0 - 1.0;
    
    vec3 Q1  = dFdx(inPos);
    vec3 Q2  = dFdy(inPos);
    vec2 st1 = dFdx(inUV);
    vec2 st2 = dFdy(inUV);

    vec3 N   = normalize(inNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    Material material = pushConstant.materialBuffer.materials[pushConstant.materialIndex];
    
    bool useBaseTexture = material.baseTextureIndex != -1;
    bool useEmissiveTexture = material.emissiveTextureIndex != -1;
    bool useOcclusionTexture = material.occlusionTextureIndex != -1;
    bool useRoughnessTexture = material.roughnessTextureIndex != -1;
    bool useNormalTexture = material.normalTextureIndex != -1;
    
    vec4 baseColor = material.baseColorFactor;
    if(useBaseTexture)
    {
        Texture tex = pushConstant.texBuffer.textures[material.baseTextureIndex];
        baseColor = pow(texture(samplers[tex.imageIndex], inUV), vec4(2.2));
    }

    vec4 emissive = vec4(0);
    if(useEmissiveTexture)
    {
        Texture tex = pushConstant.texBuffer.textures[material.emissiveTextureIndex];
        emissive = pow(texture(samplers[tex.imageIndex], inUV), vec4(2.2));
    }

    float occlusion = 1.f;
    float metal = material.metallicFactor;
    float rough = material.roughnessFactor;
    if(useRoughnessTexture)
    {
        Texture tex = pushConstant.texBuffer.textures[material.roughnessTextureIndex];
        vec4 orm = pow(texture(samplers[tex.imageIndex], inUV), vec4(2.2));
        occlusion = orm.r;
        rough = orm.g * material.roughnessFactor;
        metal = orm.b * material.metallicFactor;
    }

    if(useOcclusionTexture)
    {
        Texture tex = pushConstant.texBuffer.textures[material.occlusionTextureIndex];
        occlusion = pow(texture(samplers[tex.imageIndex], inUV), vec4(2.2)).r;
    }
    occlusion *= material.ao;

    vec3 N = inNormal;
    if(useNormalTexture)
    {
        Texture tex = pushConstant.texBuffer.textures[material.normalTextureIndex];
        N = GetNormalFromMap(tex);
    }

    vec3 V = normalize(inCamPos - inPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, vec3(baseColor), material.metallicFactor);

    vec3 Lo = vec3(0);
    #pragma unroll
    for (int i = 0; i < inLightCount; i++)
    {
        Light light = lights[i];
        vec3 lightToModel = light.position - inPos;
        vec3 L = normalize(lightToModel);
        vec3 H = normalize(V + L);
        float distance = length(lightToModel);
        float attenutation = 1 / distance * distance;
        vec3 radiance = light.color * attenutation * light.intensity;

        float NDF = DistributionGGX(N, H, rough);
        float G = GeometrySmith(N, V, L, rough);
        vec3 F = FresnelShlick(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metal;

        vec3 num = NDF * G * F;
        float denom = 4 * max(dot(N, V), 0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = num / denom;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * vec3(baseColor) / PI + specular) * radiance * NdotL;
    }
    vec3 ambient = vec3(0.03) * vec3(baseColor) * occlusion;
    vec3 color = ambient + Lo + emissive.rgb;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    outFragColor = vec4(color, 1.0);
}
