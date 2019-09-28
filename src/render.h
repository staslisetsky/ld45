enum draw_mode_ {
    DrawMode_Quad,
    DrawMode_Triangle,
    DrawMode_Strip,
};

struct command_data {
    GLuint Shader;
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

struct render {
    v2i Screen;

    GLuint VertexArrayPlain;
    GLuint VertexArrayTextured;
    GLuint VertexBufferPlain;
    GLuint VertexBufferTextured;
    GLuint ViewUniformBuffer;

    GLuint PlainShader;
    GLuint TexturedShader;
    GLuint GlyphShader;
    GLuint SDFShader;

    GLuint QuadDimUniform;

    GLuint TestTexture;

    m4x4 ProjectionMatrix;

    vertex_xyzrgba *PlainVertices;
    u32 PlainVertexCount;

    vertex_xyzrgbauv *TexturedVertices;
    u32 TexturedVertexCount;

    render_command *Commands;
    u32 CommandCount;
};

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
AddRenderCommand(render *Render, draw_mode_ Mode, u32 Offset, u32 PrimitiveCount, command_data Data)
{
    Assert(Render->CommandCount + 1 <= COMMAND_BUFFER_SIZE);

    render_command *Command = 0;
    if (!Render->CommandCount) {
        Command = Render->Commands + 0;
        Command->PrimitiveCount = PrimitiveCount;
        Command->Offset = Offset;
        Command->DrawMode = Mode;
        Command->Data = Data;
        ++Render->CommandCount;
    } else {
        render_command *LastCommand = Render->Commands + Render->CommandCount - 1;

        if (LastCommand->Data.Shader != Data.Shader ||
            LastCommand->Data.Texture != Data.Texture ||
            (LastCommand->Data.QuadDim.x != Data.QuadDim.x || LastCommand->Data.QuadDim.y != Data.QuadDim.y))
        {
            Command = Render->Commands + Render->CommandCount;
            Command->PrimitiveCount = PrimitiveCount;
            Command->Offset = Offset;
            Command->DrawMode = Mode;
            Command->Data = Data;
            ++Render->CommandCount;
        } else {
            LastCommand->PrimitiveCount += PrimitiveCount;
        }
    }
}

void
DrawRect(render *Render, v4 Color, v2 P, v2 Dim, r32 Z)
{
    vertex_xyzrgba *Vertices = Render->PlainVertices + Render->PlainVertexCount;

    Assert(Render->PlainVertexCount + 6 <= VERTEX_BUFFER_SIZE);

    Vertices[0].P = v3{P.x, P.y, Z};
    Vertices[1].P = v3{P.x + Dim.x, P.y, Z};
    Vertices[2].P = v3{P.x, P.y + Dim.y, Z};
    Vertices[3].P = v3{P.x + Dim.x, P.y, Z};
    Vertices[4].P = v3{P.x, P.y + Dim.y, Z};
    Vertices[5].P = v3{P.x + Dim.x, P.y + Dim.y, Z};

    Vertices[0].Color = Color;
    Vertices[1].Color = Color;
    Vertices[2].Color = Color;
    Vertices[3].Color = Color;
    Vertices[4].Color = Color;
    Vertices[5].Color = Color;

    command_data Data = {};
    Data.Shader = Render->PlainShader;
    AddRenderCommand(Render, DrawMode_Triangle, Render->PlainVertexCount, 2, Data);
    Render->PlainVertexCount += 6;
}

void
DrawPlayer(render *Render, v4 Color, v2 P, v2 MouseP, r32 NoseRadius, r32 Z)
{
    vertex_xyzrgba *Vertices = Render->PlainVertices + Render->PlainVertexCount;

    Assert(Render->PlainVertexCount + 4 <= VERTEX_BUFFER_SIZE);

    v2 NoseVector = Normalize(MouseP - P);
    v2 WidthVector = Perp(NoseVector);

    r32 Tail = 0.3f;
    r32 Width = 0.3f;

    v2 SideVector1 = WidthVector * Width - NoseVector * Tail;
    v2 SideVector2 = -1.0f * WidthVector * Width - NoseVector * Tail;

    Vertices[0].P = V3(P + NoseVector * NoseRadius, Z);
    Vertices[1].P = V3(P + SideVector1 * NoseRadius, Z);
    Vertices[2].P = V3(P + SideVector2 * NoseRadius, Z);

    Vertices[0].Color = Color;
    Vertices[1].Color = Color;
    Vertices[2].Color = Color;

    command_data Data = {};
    Data.Shader = Render->PlainShader;
    AddRenderCommand(Render, DrawMode_Triangle, Render->PlainVertexCount, 1, Data);

    Render->PlainVertexCount += 3;
}

void
DrawTexturedRect(render *Render, v2 P, v2 Dim, v4 Color, u32 Texture, u32 Z=0)
{
    vertex_xyzrgbauv *Vertices = Render->TexturedVertices + Render->TexturedVertexCount;

    Assert(Render->TexturedVertexCount + 4 <= VERTEX_BUFFER_SIZE);

    Vertices[0].P = v3{P.x, P.y, (r32)Z};
    Vertices[1].P = v3{P.x + Dim.x, P.y, (r32)Z};
    Vertices[2].P = v3{P.x, P.y + Dim.y, (r32)Z};
    Vertices[3].P = v3{P.x + Dim.x, P.y + Dim.y, (r32)Z};

    Vertices[0].Color = Color;
    Vertices[1].Color = Color;
    Vertices[2].Color = Color;
    Vertices[3].Color = Color;

    Vertices[0].UV = v2{0.0f, 0.0f};
    Vertices[1].UV = v2{1.0f, 0.0f};
    Vertices[2].UV = v2{0.0f, 1.0f};
    Vertices[3].UV = v2{1.0f, 1.0f};

    command_data Data = {};
    Data.Shader = Render->TexturedShader;
    Data.Texture = Render->TestTexture;
    AddRenderCommand(Render, DrawMode_Strip, Render->TexturedVertexCount, 4, Data);
    Render->TexturedVertexCount += 4;
}

void
DrawGlyph(render *Render, v2 P, v2 Dim, v4 Color, u32 Texture, u32 Z=0)
{
    vertex_xyzrgbauv *Vertices = Render->TexturedVertices + Render->TexturedVertexCount;

    Assert(Render->TexturedVertexCount + 4 <= VERTEX_BUFFER_SIZE);

    Vertices[0].P = v3{P.x, P.y, (r32)Z};
    Vertices[1].P = v3{P.x + Dim.x, P.y, (r32)Z};
    Vertices[2].P = v3{P.x, P.y + Dim.y, (r32)Z};
    Vertices[3].P = v3{P.x + Dim.x, P.y + Dim.y, (r32)Z};

    Vertices[0].Color = Color;
    Vertices[1].Color = Color;
    Vertices[2].Color = Color;
    Vertices[3].Color = Color;

    Vertices[0].UV = v2{0.0f, 0.0f};
    Vertices[1].UV = v2{1.0f, 0.0f};
    Vertices[2].UV = v2{0.0f, 1.0f};
    Vertices[3].UV = v2{1.0f, 1.0f};

    command_data Data = {};
    Data.Shader = Render->SDFShader;
    Data.Texture = Render->TestTexture;
    Data.QuadDim = v2i{(s32)Dim.x, (s32)Dim.y};

    AddRenderCommand(Render, DrawMode_Strip, Render->TexturedVertexCount, 4, Data);
    Render->TexturedVertexCount += 4;
}

void
DrawText(render *Render, v2 P, r32 Z, r32 Scale, v4 Color, cached_font *Font, char *Text)
{
    u32 Len = strlen(Text);

    v2 CurrentP = P;
    u32 PreviousCodePoint = 0;

    for (u32 i=0; i<Len; ++i) {
        vertex_xyzrgbauv *Vertices = Render->TexturedVertices + Render->TexturedVertexCount;

        Assert(Render->TexturedVertexCount + 6 <= VERTEX_BUFFER_SIZE);

        cached_glyph *Glyph = GetCachedGlyph(Font, Text[i]);

        r32 XKern = GetKerningForPair(Font, PreviousCodePoint, Glyph->CodePoint);
        r32 Left = XKern + Glyph->LeftBearing;
        r32 Right = Glyph->XAdvance - (Glyph->Width + Glyph->LeftBearing);
        r32 Width = (Glyph->Width + Left + Right) * Scale;
        v2 QuadDim = v2{ (r32)Glyph->BitmapWidth, (r32)Glyph->BitmapHeight } * Scale;

        v2 GlyphP = CurrentP;
        GlyphP.x += Left * Scale;
        GlyphP.y += (Font->Baseline - Glyph->BitmapTop) * Scale;

        Vertices[0].P = v3{GlyphP.x, GlyphP.y, (r32)Z};
        Vertices[1].P = v3{GlyphP.x + QuadDim.x, GlyphP.y, (r32)Z};
        Vertices[2].P = v3{GlyphP.x, GlyphP.y + QuadDim.y, (r32)Z};

        Vertices[3].P = v3{GlyphP.x + QuadDim.x, GlyphP.y, (r32)Z};
        Vertices[4].P = v3{GlyphP.x, GlyphP.y + QuadDim.y, (r32)Z};
        Vertices[5].P = v3{GlyphP.x + QuadDim.x, GlyphP.y + QuadDim.y, (r32)Z};

        Vertices[0].Color = Color;
        Vertices[1].Color = Color;
        Vertices[2].Color = Color;
        Vertices[3].Color = Color;
        Vertices[4].Color = Color;
        Vertices[5].Color = Color;

        Vertices[0].UV = Glyph->UV.TopLeft;
        Vertices[1].UV = Glyph->UV.TopRight;
        Vertices[2].UV = Glyph->UV.BottomLeft;
        Vertices[3].UV = Glyph->UV.TopRight;
        Vertices[4].UV = Glyph->UV.BottomLeft;
        Vertices[5].UV = Glyph->UV.BottomRight;

        command_data Data = {};
        Data.Shader = Render->GlyphShader;
        Data.Texture = Font->Atlas.Texture;

        AddRenderCommand(Render, DrawMode_Quad, Render->TexturedVertexCount, 1, Data);
        Render->TexturedVertexCount += 6;

        CurrentP.x += Width;
        PreviousCodePoint = Glyph->CodePoint;
    }
}

void
DrawText(render *Render, v2 P, v4 Color, font_ FontId, r32 SizePt, char *Text)
{
    cached_font *Font = 0;

    static cached_font *LastFont = 0;

    s32 Index = -1;
    r32 MinSizeDiff = 999999.0f;
    for (u32 i=0; i<CachedFontCount; ++i) {
        cached_font *Font = FontCache + i;
        if (Font->Id == FontId) {
            r32 Diff = Abs_r32(SizePt - Font->SizePt);
            if (Diff < MinSizeDiff) {
                Index = i;
                MinSizeDiff = Diff;
            }
        }
    }

    if (Index >=0) {
        Font = FontCache + Index;
    }

    // if (Font != LastFont) {
    //     LastFont = Font;
    //     char Buf[50];
    //     sprintf(Buf, "Switch to font: %.2f\n", Font->SizePt);
    //     OutputDebugString(Buf);
    // }

    Assert(Font);

    r32 Scale = SizePt / Font->SizePt;
    DrawText(Render, P, 1.0f, Scale, Color, Font, Text);
}
