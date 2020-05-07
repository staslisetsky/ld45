r32
TextSize(cached_font *Font, r32 Scale, char *Text, u32 Len)
{
    r32 Size = 0.0f;
    u32 PreviousCodePoint = 0;

    for (u32 i=0; i<Len; ++i) {
        cached_glyph *Glyph = GetCachedGlyph(Font, Text[i]);

        r32 XKern = GetKerningForPair(Font, PreviousCodePoint, Glyph->CodePoint);
        r32 Left = XKern + Glyph->LeftBearing;
        r32 Right = Glyph->XAdvance - (Glyph->Width + Glyph->LeftBearing);
        r32 Width = (Glyph->Width + Left + Right) * Scale;

        Size += Width;
    }

    return Size;
}

r32
WordSize(cached_font *Font, r32 Scale, char *Text, u32 Len)
{
    r32 Size = 0.0f;
    u32 PreviousCodePoint = 0;

    for (u32 i=0; i<Len; ++i) {
        if (Text[i] == ' ') {
            break;
        }
        cached_glyph *Glyph = GetCachedGlyph(Font, Text[i]);

        r32 XKern = GetKerningForPair(Font, PreviousCodePoint, Glyph->CodePoint);
        r32 Left = XKern + Glyph->LeftBearing;
        r32 Right = Glyph->XAdvance - (Glyph->Width + Glyph->LeftBearing);
        r32 Width = (Glyph->Width + Left + Right) * Scale;

        Size += Width;
    }

    return Size;
}

// void
// DrawText(v2 P, v4 Color, font_ FontId, r32 SizePx, char *Text, r32 Len)
// {
//     r32 Scale;
//     cached_font *Font = FindMatchingFont(FontId, SizePx, &Scale);
//     DrawText(P, 1.0f, Scale, Color, Font, Text, Len);
// }

// rect
// TextLayout(text_ TextType, char *Text, u32 FullLen, u32 PrintLen)
// {
//     r32 Scale;
//     cached_font *Font = FindMatchingFont(Font_PTSans, 25.0f, &Scale);

//     b32 Wrapped = false;
//     b32 WordBreakFound = false;
//     u32 PreviousCodePoint = 0;

//     r32 Z = 1.0;

//     v4 Color = RGBA(255,255,255,255);

//     if (TextType == Text_Play) {
//         Color = RGBA(100,255,95,255);
//     } else if (TextType == Text_No) {
//         Color = RGBA(255,111,95,255);
//     }

//     rect AABB = {};
//     AABB.Min = State.P;

//     for (u32 i=0; i<PrintLen; ++i) {
//         if (Text[i] == '\n') {
//             State.P.x = State.Layout.Min.x;
//             State.P.y += Font->BaselineSpacing * Scale * 1.3f;

//             PreviousCodePoint = 10;
//             continue;
//         }

//         if (PreviousCodePoint == 10 || PreviousCodePoint == 32) {
//             r32 SpaceLeft = State.Layout.Max.x - State.P.x;
//             r32 WS = WordSize(Font, Scale, Text + i, FullLen - i);
//             if (WS > SpaceLeft) {
//                 // wrap
//                 State.P.x = State.Layout.Min.x;
//                 State.P.y += Font->BaselineSpacing * Scale * 1.3f;
//             }
//         }

//         vertex_xyzrgbauv *Vertices = Render.TexturedVertices + Render.TexturedVertexCount;

//         Assert(Render.TexturedVertexCount + 6 <= VERTEX_BUFFER_SIZE);

//         cached_glyph *Glyph = GetCachedGlyph(Font, Text[i]);

//         v2 TextureOffsetPx = {1.0, 1.0f};

//         r32 XKern = GetKerningForPair(Font, PreviousCodePoint, Glyph->CodePoint);
//         r32 Left = XKern + Glyph->LeftBearing;
//         r32 Right = Glyph->XAdvance - (Glyph->Width + Glyph->LeftBearing);
//         r32 Width = (Glyph->Width + Left + Right) * Scale;
//         v2 QuadDim = v2{(r32)Glyph->BitmapWidth, (r32)Glyph->BitmapHeight} * Scale;

//         QuadDim.x += TextureOffsetPx.x;
//         QuadDim.y += TextureOffsetPx.y;

//         v2 GlyphP = State.P;
//         GlyphP.x += Left * Scale - TextureOffsetPx.x;
//         GlyphP.y += (Font->Baseline - Glyph->BitmapTop) * Scale - TextureOffsetPx.y;

//         Vertices[0].P = v3{GlyphP.x, GlyphP.y, (r32)Z};
//         Vertices[1].P = v3{GlyphP.x + QuadDim.x, GlyphP.y, (r32)Z};
//         Vertices[2].P = v3{GlyphP.x, GlyphP.y + QuadDim.y, (r32)Z};

//         Vertices[3].P = v3{GlyphP.x + QuadDim.x, GlyphP.y, (r32)Z};
//         Vertices[4].P = v3{GlyphP.x, GlyphP.y + QuadDim.y, (r32)Z};
//         Vertices[5].P = v3{GlyphP.x + QuadDim.x, GlyphP.y + QuadDim.y, (r32)Z};

//         Vertices[0].Color = Color / 255.0f;
//         Vertices[1].Color = Color / 255.0f;
//         Vertices[2].Color = Color / 255.0f;
//         Vertices[3].Color = Color / 255.0f;
//         Vertices[4].Color = Color / 255.0f;
//         Vertices[5].Color = Color / 255.0f;

//         v2 TexelOffset = { TextureOffsetPx.x / (Font->Atlas.Width * Scale), TextureOffsetPx.y / (Font->Atlas.Height * Scale) };

//         Vertices[0].UV = Glyph->UV.TopLeft - TexelOffset;
//         Vertices[1].UV = Glyph->UV.TopRight + v2{TexelOffset.x, -TexelOffset.y};
//         Vertices[2].UV = Glyph->UV.BottomLeft + v2{-TexelOffset.x, TexelOffset.y};
//         Vertices[3].UV = Glyph->UV.TopRight + v2{TexelOffset.x, -TexelOffset.y};
//         Vertices[4].UV = Glyph->UV.BottomLeft + v2{-TexelOffset.x, TexelOffset.y};
//         Vertices[5].UV = Glyph->UV.BottomRight + TexelOffset;

//         command_data Data = {};
//         Data.Shader = Shader_Glyph;
//         Data.Texture = Font->Atlas.Texture;

//         AddRenderCommand(DrawMode_Quad, Render.TexturedVertexCount, 1, Data);
//         Render.TexturedVertexCount += 6;

//         State.P.x += Width;
//         PreviousCodePoint = Glyph->CodePoint;
//     }

//     AABB.Max = State.P;
//     AABB.Max.y += Font->BaselineSpacing * Scale * 1.3f;

//     return AABB;
// }