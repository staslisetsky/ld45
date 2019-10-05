#version 300 es

precision mediump float;
uniform sampler2D TextureSample;
in vec4 VertexColor;
in vec2 TexelUV;
out vec4 FragmentColor;

float GammaUp(float Component)
{
    float Result = 0.0;

    if (Component <= 0.04045) {
        Result = Component / 12.92;
    } else {
        Result = pow((Component + 0.055) / 1.055, 2.4);
    }

    return (Result);
}

void main(void)
{
    vec4 ColorSample = texture(TextureSample, TexelUV);
    //FragmentColor = vec4(VertexColor.rgb, ColorSample.r * VertexColor.a);
    FragmentColor = vec4(VertexColor.rgb, GammaUp(ColorSample.r * VertexColor.a));
}