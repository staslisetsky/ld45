enum font_ {
    Font_PTSans,
    Font_PTSansCaption,
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

    r32 SizePx;
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
    r32 SizePx;
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


static cached_font FontCache[20];
static u32 CachedFontCount = 0;

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

cached_font *
FindMatchingFont(font_ FontId, r32 SizePx,r32 *Scale)
{
    cached_font *Font = 0;

    static cached_font *LastFont = 0;

    s32 Index = -1;
    r32 MinSizeDiff = 999999.0f;
    for (u32 i=0; i<CachedFontCount; ++i) {
        cached_font *Font = FontCache + i;
        if (Font->Id == FontId) {
            r32 Diff = Abs_r32(SizePx - Font->SizePx);
            if (Diff < MinSizeDiff) {
                Index = i;
                MinSizeDiff = Diff;
            }
        }
    }

    if (Index >=0 ) {
        Font = FontCache + Index;
    }

    *Scale = SizePx / Font->SizePx;

    Assert(Font);

    return Font;
}

v2
GetTextDim(font_ FontId, r32 SizePx, char *Text)
{
    v2 Dim = {};

    r32 Scale;
    cached_font *Font = FindMatchingFont(FontId, SizePx, &Scale);

    u32 Len = strlen(Text);
    u32 PreviousCodePoint = 0;

    for (u32 i=0; i<Len; ++i) {
        cached_glyph *Glyph = GetCachedGlyph(Font, Text[i]);

        r32 XKern = GetKerningForPair(Font, PreviousCodePoint, Glyph->CodePoint);
        r32 Left = XKern + Glyph->LeftBearing;
        r32 Right = Glyph->XAdvance - (Glyph->Width + Glyph->LeftBearing);
        r32 Width = (Glyph->Width + Left + Right) * Scale;

        Dim.x += Width;
        Dim.y = Max_r32(Dim.y, Glyph->Height * Scale);

        if (Layout.FontSpacing > 0.0f) {
            Dim.x += Width * Layout.FontSpacing;
        }

        PreviousCodePoint = Glyph->CodePoint;
    }

    return Dim;
}
