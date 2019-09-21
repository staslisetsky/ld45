#version 330 core

layout (location = 0) in vec4 in_VertexP;
layout (location = 1) in vec2 In_TexelUV;

layout(std140) uniform view
{
    mat4 Projection;
} View;

out vec4 P;
out vec2 TexelUV;

void main(void)
{
    P = in_VertexP;
    TexelUV = In_TexelUV;
    gl_Position = View.Projection * in_VertexP;
}
