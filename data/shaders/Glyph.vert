#version 330 core

layout (location = 0) in vec4 In_P;
layout (location = 1) in vec4 In_VertexColor;
layout (location = 2) in vec2 In_TexelUV;

out vec2 TexelUV;
out vec4 Color;

layout(std140) uniform view
{
    mat4 Projection;
} View;

void main(void)
{
    gl_Position = View.Projection * In_P;
    TexelUV = In_TexelUV;
    Color = In_VertexColor;
}
