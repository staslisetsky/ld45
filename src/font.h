struct codepoint_pair {
    u16 UnicodeCodepointA;
    u16 UnicodeCodepointB;
    r32 HAdvance;
};

struct uincode_character_map {
    u32 Index;
    u32 CodePoint;
};

struct cached_glyph {
    u16 CodePoint;

    r32 Width;
    r32 Height;

    r32 LeftBearing;
    r32 TopBearing;
    r32 XAdvance;

    u32 Texture;
    uv UV;
};

struct bitmap {
    u8 *Data;
    u32 Width;
    u32 Height;
};

struct cached_font {
    char Name[20];
    bitmap Atlas;

    r32 SizePt;
    r32 PxPerFontUnit;

    r32 Height;
    r32 Baseline;
    r32 BaselineSpacing;
    r32 Ascender;
    r32 Descender;

    uincode_character_map *Map;
    cached_glyph *Glyphs;
    r32 *Advances;
    u32 GlyphCount;
};

struct file_header {
    char M[5];
    u32 Count;
};

u32
LocateCodepointIndex(cached_font *Font, u16 CodePoint)
{
    u32 Result = 0;
    for (u32 i = 0; i < Font->GlyphCount; ++i) {
        if (Font->Map[i].CodePoint == CodePoint) {
            Result = i;
            break;
        }
    }

    return Result;
}

cached_glyph *
GetCachedGlyph(cached_font *Font, u32 Codepoint)
{
    cached_glyph *Result;
    u32 Index = LocateCodepointIndex(Font, Codepoint);
    return Font->Glyphs + Index;
}

r32
GetKerningForPair(cached_font *Font, u16 CodePointA, u16 CodePointB)
{
    r32 Result = 0.0f;

    u32 CodePointAIndex = LocateCodepointIndex(Font, CodePointA);
    u32 CodePointBIndex = LocateCodepointIndex(Font, CodePointB);

    // Result = Font->Advances[CodePointAIndex * Font->BaseFont->CodePointCount + CodePointBIndex] / 64.0f;

    return Result;
}