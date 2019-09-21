#version 330 core

uniform sampler2D TextureSample;

in vec2 TexelUV;
in vec4 Color;

out vec4 Result;

float LinearToSRGB(float Component)
{
    float Result = 0.0;

    if (Component <= 0.0031308) {
        Result = Component * 12.92;
    } else {
        Result = 1.055 * pow(Component, 0.41666) - 0.055;
    }

    return (Result);
}

float SRGBToLinear(float Component)
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
    vec4 TextureColor = texture(TextureSample, TexelUV);
    float Coverage = TextureColor.r;
    Result = vec4(Color.rgb, Coverage * Color.a);
}

