#version  460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in float inUVx;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in float inUVy;
layout(location = 4) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = vec4(inPosition, 1.0);
    fragColor = inColor;
}