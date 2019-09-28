#version 300 es

precision mediump float;
uniform sampler2D TextureSample;
in vec4 VertexColor;
in vec2 TexelUV;
out vec4 FragmentColor;

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

void main(void)
{
    vec4 ColorSample = texture(TextureSample, TexelUV);
    FragmentColor = vec4(VertexColor.rgb, LinearToSRGB(ColorSample.r));
    // vec3 Mixed = mix(vec3(1.0, 0.0, 1.0), VertexColor.rgb, ColorSample.r);
    // FragmentColor = vec4(Mixed, 1.0);
}