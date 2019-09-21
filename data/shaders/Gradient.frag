#version 330 core

uniform sampler2D TextureSample;
uniform float TextureOpacity;

in vec2 TexelUV;

out vec4 out_Result;
uniform vec2 GradientBoxVertices[4];
uniform vec4 StopColors[20];
uniform float StopPositions[20];
uniform uint StopCount;
uniform vec2 ScreenDim;
uniform int Texture;

in vec4 P;

void main(void)
{
    float MonitorGamma = 2.2;
    
    // Vertex indices:
    // 1      2
    // -------
    // |     |
    // -------
    // 0      3

    // y in the coordiantes grows from top to bottom

    vec4 FinalColor = vec4(1.0, 1.0, 0.0, 1.0);

    vec2 InvertedYScreenP = gl_FragCoord.xy;
    InvertedYScreenP.y = ScreenDim.y - gl_FragCoord.y;
    
    vec2 WidthEdge = (GradientBoxVertices[2] - GradientBoxVertices[1]);
    float EdgeLength = length(WidthEdge);
    vec2 WidthEdgeNormalized = WidthEdge / EdgeLength;

    vec2 Diff = InvertedYScreenP.xy - GradientBoxVertices[1];

    float RelativeX = dot(Diff, WidthEdgeNormalized) / EdgeLength;
    float SegmentNormalizedX = RelativeX;

    vec4 From = StopColors[0] / 255.0;
    vec4 To = StopColors[1] / 255.0;

    bool UseSingleColor = false;
    vec4 SingleColor = vec4(1.0,0.0,0.0,1.0);

    if (RelativeX < StopPositions[0]) {
        UseSingleColor = true;
        SingleColor = StopColors[0] / 255.0;
    } else if (RelativeX >= StopPositions[StopCount - uint(1)]) {
        UseSingleColor = true;
        SingleColor = StopColors[StopCount - uint(1)] / 255.0;
    }

    if (!UseSingleColor) {
        for (uint i = uint(0); i < StopCount - uint(1); ++i) {
            if ((RelativeX >= StopPositions[i]) && (RelativeX < StopPositions[i + uint(1)])) {
                From = StopColors[i] / 255.0;
                To = StopColors[i + uint(1)] / 255.0;
                SegmentNormalizedX = (RelativeX - StopPositions[i]) / (StopPositions[i + uint(1)] - StopPositions[i]);
            }
        }
    }

    if (UseSingleColor) {
        SingleColor.rgb = pow(SingleColor.rgb, vec3(2.2));
        FinalColor = SingleColor;
    } else {
        From.rgb = pow(From.rgb, vec3(2.2));
        To.rgb = pow(To.rgb, vec3(2.2));
        FinalColor = mix(From, To, SegmentNormalizedX);
    }

    if (Texture > int(0)) {
        vec4 LinearTextureSample = texture(TextureSample, TexelUV);
        FinalColor.rgb = LinearTextureSample.rgb + (FinalColor.rgb * (1.0 - LinearTextureSample.a));
        FinalColor.a = LinearTextureSample.a + (FinalColor.a * (1.0 - LinearTextureSample.a));
    }

    out_Result = FinalColor;
}
