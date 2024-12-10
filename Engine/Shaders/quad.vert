#version 450

const vec2 positions[6] = vec2[6](
// Triangle 1
vec2(-0.5,  0.5), // Top-left
vec2( 0.5,  0.5), // Top-right
vec2(-0.5, -0.5), // Bottom-left
// Triangle 2
vec2( 0.5,  0.5), // Top-right
vec2( 0.5, -0.5), // Bottom-right
vec2(-0.5, -0.5)  // Bottom-left
);

const vec3 colors[6] = vec3[6](
// Triangle 1
vec3(1.0, 0.0, 0.0), // Red
vec3(0.0, 1.0, 0.0), // Green
vec3(0.0, 0.0, 1.0), // Blue
// Triangle 2
vec3(0.0, 1.0, 0.0), // Green
vec3(1.0, 1.0, 0.0), // Yellow
vec3(0.0, 0.0, 1.0)  // Blue
);

const vec2 uvs[6] = vec2[6](
// Triangle 1
vec2(0.0, 1.0), // Top-left
vec2(1.0, 1.0), // Top-right
vec2(0.0, 0.0), // Bottom-left
// Triangle 2
vec2(1.0, 1.0), // Top-right
vec2(1.0, 0.0), // Bottom-right
vec2(0.0, 0.0)  // Bottom-left
);

// Output to fragment shader
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 outUV;

void main() {
    fragColor = colors[gl_VertexIndex];
    outUV = uvs[gl_VertexIndex];

    // Set the position of the vertex in clip space
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
