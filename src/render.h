struct image {
    u32 Texture;

    u32 Width;
    u32 Height;
    u32 N;

    u8 *Data;
};

enum shader_ {
    Shader_Plain,
    Shader_Textured,
    Shader_Glyph,
    Shader_Count,
};

enum draw_mode_ {
    DrawMode_Quad,
    DrawMode_Triangle,
    DrawMode_Strip,
};

struct command_data {
    shader_ Shader;
    GLuint Texture;
    v2i QuadDim;
};

struct render_command {
    draw_mode_ DrawMode;
    u32 Offset;
    u32 PrimitiveCount;
    command_data Data;
};

struct vertex_xyzrgba {
    v3 P;
    v4 Color;
};

struct vertex_xyzrgbauv {
    v3 P;
    v4 Color;
    v2 UV;
};

struct shader {
    ls_static_string<32> Name;
    GLuint Id;
};

struct render {
    char *ShaderError;

    v2i Screen;

    GLuint VertexArrayPlain;
    GLuint VertexArrayTextured;
    GLuint VertexBufferPlain;
    GLuint VertexBufferTextured;
    GLuint ViewUniformBuffer;

    shader Shaders[Shader_Count];
    GLuint QuadDimUniform;

    m4x4 ProjectionMatrix;

    vertex_xyzrgba *PlainVertices;
    u32 PlainVertexCount;

    vertex_xyzrgbauv *TexturedVertices;
    u32 TexturedVertexCount;

    render_command *Commands;
    u32 CommandCount;
};

static render Render = {};

#define VERTEX_BUFFER_SIZE 1000
#define COMMAND_BUFFER_SIZE 100

m4x4
GetOrthoProjectionMatrix(r32 Near, r32 Far, r32 ScreenWidth, r32 ScreenHeight)
{
    m4x4 Result = {};

    r32 a = SafeDivide1(2.0f, ScreenWidth);
    r32 b = SafeDivide1(2.0f, ScreenHeight);
    r32 c = SafeDivide1(1.0f, Far - Near);

    Result = {
        {a,  0,  0,  0,
         0, -b,  0,  0,
         0,  0, -c,  0,
        -1,  1,  0,  1
        }
    };

    return Result;
}

void
AddRenderCommand(draw_mode_ Mode, u32 Offset, u32 PrimitiveCount, command_data Data)
{
    Assert(Render.CommandCount + 1 <= COMMAND_BUFFER_SIZE);

    render_command *Command = 0;
    if (!Render.CommandCount) {
        Command = Render.Commands + 0;
        Command->PrimitiveCount = PrimitiveCount;
        Command->Offset = Offset;
        Command->DrawMode = Mode;
        Command->Data = Data;
        ++Render.CommandCount;
    } else {
        render_command *LastCommand = Render.Commands + Render.CommandCount - 1;

        if (LastCommand->Data.Shader != Data.Shader ||
            LastCommand->Data.Texture != Data.Texture ||
            (LastCommand->Data.QuadDim.x != Data.QuadDim.x || LastCommand->Data.QuadDim.y != Data.QuadDim.y))
        {
            Command = Render.Commands + Render.CommandCount;
            Command->PrimitiveCount = PrimitiveCount;
            Command->Offset = Offset;
            Command->DrawMode = Mode;
            Command->Data = Data;
            ++Render.CommandCount;
        } else {
            LastCommand->PrimitiveCount += PrimitiveCount;
        }
    }
}

void
DrawRect(v4 Color, v2 P, v2 Dim, r32 Z)
{
    vertex_xyzrgba *Vertices = Render.PlainVertices + Render.PlainVertexCount;

    Assert(Render.PlainVertexCount + 6 <= VERTEX_BUFFER_SIZE);

    Vertices[0].P = v3{P.x, P.y, Z};
    Vertices[1].P = v3{P.x + Dim.x, P.y, Z};
    Vertices[2].P = v3{P.x, P.y + Dim.y, Z};
    Vertices[3].P = v3{P.x + Dim.x, P.y, Z};
    Vertices[4].P = v3{P.x, P.y + Dim.y, Z};
    Vertices[5].P = v3{P.x + Dim.x, P.y + Dim.y, Z};

    Vertices[0].Color = Color / 255.0f;
    Vertices[1].Color = Color / 255.0f;
    Vertices[2].Color = Color / 255.0f;
    Vertices[3].Color = Color / 255.0f;
    Vertices[4].Color = Color / 255.0f;
    Vertices[5].Color = Color / 255.0f;

    command_data Data = {};
    Data.Shader = Shader_Plain;
    AddRenderCommand(DrawMode_Triangle, Render.PlainVertexCount, 2, Data);
    Render.PlainVertexCount += 6;
}

// void
// DrawTexturedRect(render *Render, v2 P, v2 Dim, v4 Color, u32 Texture, u32 Z=0)
// {
//     vertex_xyzrgbauv *Vertices = Render.TexturedVertices + Render.TexturedVertexCount;

//     Assert(Render.TexturedVertexCount + 4 <= VERTEX_BUFFER_SIZE);

//     Vertices[0].P = v3{P.x, P.y, (r32)Z};
//     Vertices[1].P = v3{P.x + Dim.x, P.y, (r32)Z};
//     Vertices[2].P = v3{P.x, P.y + Dim.y, (r32)Z};
//     Vertices[3].P = v3{P.x + Dim.x, P.y + Dim.y, (r32)Z};

//     Vertices[0].Color = Color / 255.0f;
//     Vertices[1].Color = Color / 255.0f;
//     Vertices[2].Color = Color / 255.0f;
//     Vertices[3].Color = Color / 255.0f;

//     Vertices[0].UV = v2{0.0f, 0.0f};
//     Vertices[1].UV = v2{1.0f, 0.0f};
//     Vertices[2].UV = v2{0.0f, 1.0f};
//     Vertices[3].UV = v2{1.0f, 1.0f};

//     command_data Data = {};
//     Data.Shader = Shader_Textured;

//     assert(!"texture is wrong! fix me!");
//     // Data.Texture = Render.TestTexture;
//     AddRenderCommand(Render, DrawMode_Strip, Render.TexturedVertexCount, 4, Data);
//     Render.TexturedVertexCount += 4;
// }

void
DrawText(v2 P, r32 Z, r32 Scale, v4 Color, cached_font *Font, char *Text, r32 Len)
{
    v2 CurrentP = P;
    u32 PreviousCodePoint = 0;

    u32 IntegerLen = (u32)(Len);

    for (u32 i=0; i<Len; ++i) {
        v4 GlyphColor = Color;

        if (i == IntegerLen) {
            GlyphColor.a *= Len - IntegerLen;
        }

        vertex_xyzrgbauv *Vertices = Render.TexturedVertices + Render.TexturedVertexCount;

        Assert(Render.TexturedVertexCount + 6 <= VERTEX_BUFFER_SIZE);

        cached_glyph *Glyph = GetCachedGlyph(Font, Text[i]);

        v2 TextureOffsetPx = {1.0, 1.0f};

        r32 XKern = GetKerningForPair(Font, PreviousCodePoint, Glyph->CodePoint);
        r32 Left = XKern + Glyph->LeftBearing;
        r32 Right = Glyph->XAdvance - (Glyph->Width + Glyph->LeftBearing);
        r32 Width = (Glyph->Width + Left + Right) * Scale;
        v2 QuadDim = v2{(r32)Glyph->BitmapWidth, (r32)Glyph->BitmapHeight} * Scale;
        QuadDim.x += TextureOffsetPx.x;
        QuadDim.y += TextureOffsetPx.y;

        v2 GlyphP = CurrentP;
        GlyphP.x += Left * Scale - TextureOffsetPx.x;
        GlyphP.y += (Font->Baseline - Glyph->BitmapTop) * Scale - TextureOffsetPx.y;

        Vertices[0].P = v3{GlyphP.x, GlyphP.y, (r32)Z};
        Vertices[1].P = v3{GlyphP.x + QuadDim.x, GlyphP.y, (r32)Z};
        Vertices[2].P = v3{GlyphP.x, GlyphP.y + QuadDim.y, (r32)Z};

        Vertices[3].P = v3{GlyphP.x + QuadDim.x, GlyphP.y, (r32)Z};
        Vertices[4].P = v3{GlyphP.x, GlyphP.y + QuadDim.y, (r32)Z};
        Vertices[5].P = v3{GlyphP.x + QuadDim.x, GlyphP.y + QuadDim.y, (r32)Z};

        Vertices[0].Color = GlyphColor / 255.0f;
        Vertices[1].Color = GlyphColor / 255.0f;
        Vertices[2].Color = GlyphColor / 255.0f;
        Vertices[3].Color = GlyphColor / 255.0f;
        Vertices[4].Color = GlyphColor / 255.0f;
        Vertices[5].Color = GlyphColor / 255.0f;

        v2 TexelOffset = { TextureOffsetPx.x / (Font->Atlas.Width * Scale), TextureOffsetPx.y / (Font->Atlas.Height * Scale) };

        Vertices[0].UV = Glyph->UV.TopLeft - TexelOffset;
        Vertices[1].UV = Glyph->UV.TopRight + v2{TexelOffset.x, -TexelOffset.y};
        Vertices[2].UV = Glyph->UV.BottomLeft + v2{-TexelOffset.x, TexelOffset.y};
        Vertices[3].UV = Glyph->UV.TopRight + v2{TexelOffset.x, -TexelOffset.y};
        Vertices[4].UV = Glyph->UV.BottomLeft + v2{-TexelOffset.x, TexelOffset.y};
        Vertices[5].UV = Glyph->UV.BottomRight + TexelOffset;

        command_data Data = {};
        Data.Shader = Shader_Glyph;
        Data.Texture = Font->Atlas.Texture;

        AddRenderCommand(DrawMode_Quad, Render.TexturedVertexCount, 1, Data);
        Render.TexturedVertexCount += 6;

        CurrentP.x += Width;
        if (Layout.FontSpacing > 0.0f) {
            CurrentP.x += Width * Layout.FontSpacing;
        }
        PreviousCodePoint = Glyph->CodePoint;
    }
}
