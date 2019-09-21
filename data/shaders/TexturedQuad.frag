#version 330 core

uniform sampler2D TextureSample;

in vec2 TexelUV;
in vec4 VertexColor;

out vec4 Result;

float SRGBToLinear(float Component)
{
    // note: this is from:
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

float LinearToSRGB(float Component)
{
    // note: this is from:
    // http://developer.download.nvidia.com/opengl/specs/GL_EXT_texture_sRGB.txt
    // Section 3.8.x, sRGB Texture Color Conversion

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
    vec4 Color = vec4(0.0);

    // note: I use srgb decode ext for all textures currently
    //             + textures are stored with premultiplied alpha

    vec4 LinearTextureSample = texture(TextureSample, TexelUV);
    Color = LinearTextureSample;

    // if (Color.a < 1.0) {
    //     Color.a = 0.5;
    //     Color.rgb = vec3(1.0, 0.0, 0.0);
    // }
    // if (Color.r < 1.0 && Color.a > 0.0) {
    //     Color.a = 0.5;
    //     Color.rgb = vec3(1.0, 0.0, 0.0);
    // }

    Color.r = SRGBToLinear(Color.r);
    Color.g = SRGBToLinear(Color.g);
    Color.b = SRGBToLinear(Color.b);

    // Vertex color can be used to tint the texture
    // maybe we can use additional color to set the background for transparent images (to save 1 extra quad)

    Color.a = LinearTextureSample.a * VertexColor.a;

    Result = Color;
}
