#version 460

layout(location = 0) in vec3 inColor;
layout(location = 0) out vec4 outFragColor;
layout(location = 1) in vec2 inUV;

layout(binding = 0) uniform sampler2D samplers[];

void main()
{
    outFragColor = texture(samplers[0], inUV);
}