enum font_ {
    Font_PTSans,
    Font_Count,
};

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

    u32 BitmapWidth;
    u32 BitmapHeight;
    u32 BitmapLeft;
    u32 BitmapTop;

    r32 LeftBearing;
    r32 TopBearing;
    r32 Width;
    r32 Height;
    r32 XAdvance;

    u32 Texture;
    uv UV;
};

struct bitmap {
    u8 *Data;
    u32 Width;
    u32 Height;
    u32 Texture;
};

struct cached_font {
    char Name[20];
    font_ Id;
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

struct packed_font {
    char Name[20];
    font_ Id;
    u32 GlyphCount;

    u32 AtlasWidth;
    u32 AtlasHeight;
    r32 SizePt;
    r32 PxPerFontUnit;
    r32 Height;
    r32 Baseline;
    r32 BaselineSpacing;
    r32 Ascender;
    r32 Descender;
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

    Result = Font->Advances[CodePointAIndex * Font->GlyphCount + CodePointBIndex];

    return Result;
}

static cached_font FontCache[20];
static u32 CachedFontCount = 0;