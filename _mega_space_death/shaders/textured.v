#version 300 es

in vec4 In_P;
in vec4 In_VertexColor;
in vec2 In_TexelUV;

out vec4 VertexColor;
out vec2 TexelUV;

layout(std140) uniform view
{
    mat4 Projection;
} View;

void main()
{
    gl_Position = View.Projection * In_P;
    VertexColor = In_VertexColor;
    TexelUV = In_TexelUV;
}