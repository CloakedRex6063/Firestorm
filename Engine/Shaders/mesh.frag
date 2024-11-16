#version 460

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 0) uniform sampler2D textureSampler;

void main() 
{
    outColor = inColor * texture(textureSampler, inUV);
}