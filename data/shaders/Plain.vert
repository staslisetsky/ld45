#version 330 core

layout (location = 0) in vec4 In_P;
layout (location = 1) in vec4 In_VertexColor;

layout(std140) uniform view
{
    mat4 Projection;
} View;

out vec4 VertexColor;

void main()
{
    gl_Position = View.Projection * In_P;
    VertexColor = In_VertexColor;
}
