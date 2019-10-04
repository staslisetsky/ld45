#version 300 es

precision mediump float;
in vec4 VertexColor;
out vec4 FragmentColor;
void main(void)
{
    FragmentColor = VertexColor;
}