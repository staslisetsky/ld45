#version 330 core

in vec4 VertexColor;
out vec4 Result;

float SRGBToLinear(float Component)
{
    // note(stas): this is from:
    // http://developer.download.nvidia.com/opengl/specs/GL_EXT_texture_sRGB.txt
    // Section 3.8.x, sRGB Texture Color Conversion

    float Result = 0.0;

    if (Component <= 0.04045) {
        Result = Component / 12.92;
    } else {
        Result = pow((Component + 0.055) / 1.055, 2.4);
    }

    return (Result);
}

vec3 vec3FromSRGBToLinear(vec3 Color)
{
    vec3 Result = vec3(SRGBToLinear(Color.r), SRGBToLinear(Color.g), SRGBToLinear(Color.b));
    return (Result);
}

void main(void)
{
    // vec3 Linear = vec3FromSRGBToLinear(VertexColor.rgb);
    // Result = vec4(Linear, VertexColor.a);

    Result = VertexColor;
}
