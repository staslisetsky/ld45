#version 300 es
precision mediump float;
uniform sampler2D TextureSample;
in vec4 VertexColor;
in vec2 TexelUV;
out vec4 FragmentColor;
void main(void)
{
    FragmentColor = texture(TextureSample, TexelUV);
    // FragmentColor = vec4(0.3, 0.7, 0.22, 1.0);
}