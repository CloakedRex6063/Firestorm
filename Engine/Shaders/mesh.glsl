#version 460
#extension GL_EXT_mesh_shader : require

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;
layout(triangles, max_vertices = 64, max_primitives = 126) out;

void main()
{
    uint vertexCount = 3;
    uint triangleCount = 1;
    SetMeshOutputsEXT(vertexCount, triangleCount);
    gl_MeshVerticesEXT[0].gl_Position = vec4(0.5, -0.5, 1, 1);
    gl_MeshVerticesEXT[1].gl_Position = vec4(0.5, 0.5, 1, 1);
    gl_MeshVerticesEXT[2].gl_Position = vec4(-0.5, 0.5, 1, 1);
    gl_PrimitiveTriangleIndicesEXT[0] = uvec3(0, 1, 2);
}