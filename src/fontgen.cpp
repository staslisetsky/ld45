#include <stdint.h>
#include <math.h>
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"

#include "freetype/ftadvanc.h"
#include FT_FREETYPE_H

#define Assert(Expression) if(!(Expression)) { printf("assertion failed %s:%d", __FILE__, __LINE__); *(int *)0=0; }

typedef char utf8;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s32 b32;
typedef float r32;
typedef double r64;

#include "ls_math.h"
#include "font.h"

#define SYSTEM_PPI 96.0

r32
PointsToPixels(r32 Points, r32 PPI)
{
    r32 Result = (Points / 72.0f) * SYSTEM_PPI;
    //                  in

    return(Result);
}

// r32
// PixelsToPoints(r32 Pixels, r32 PPI)
// {
//     r32 Result =  Pixels / PPI * SYSTEM_PPI;
//     return(Result);
// }

bitmap
BakeFont(cached_font Font, FT_Face Face, u32 PPI, r32 SizePx, u32 W, u32 H)
{
    bitmap Atlas = {};
    Atlas.Width = W;
    Atlas.Height = H;
    Atlas.Data = (u8 *)malloc(W * H);

    r32 FontFullHeight = Face->height * Font.PxPerFontUnit;

    u32 XOffset = 1;
    u32 YOffset = 1;

    for (u32 i = 0; i < Font.GlyphCount; ++i) {

        uincode_character_map *CharMapEntry = Font.Map + i;
        if (CharMapEntry->CodePoint > 0xb0) {
            break;
        }
        FT_Set_Char_Size(Face, 0, SizePx * 64, PPI, PPI);
        FT_Load_Glyph(Face, CharMapEntry->Index, FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT);

        FT_Glyph_Metrics Metrics = Face->glyph->metrics;

        cached_glyph *Glyph = Font.Glyphs + i;
        *Glyph = {};

        Glyph->CodePoint = CharMapEntry->CodePoint;
        Glyph->XAdvance = Metrics.horiAdvance / 64.0f;
        Glyph->Width  = Metrics.width / 64.0f;
        Glyph->Height = Metrics.height / 64.0f;


        //
        //
        //

        FT_Render_Glyph(Face->glyph, FT_RENDER_MODE_NORMAL);
        Glyph->TopBearing = (r32)Face->glyph->bitmap_top;
        Glyph->LeftBearing = (r32)Face->glyph->bitmap_left;

        u32 GlyphBitmapW = Face->glyph->bitmap.width;
        u32 GlyphBitmapH = Face->glyph->bitmap.rows;

        if (XOffset + GlyphBitmapW >= Atlas.Width - 1) {
            XOffset = 1;
            YOffset += FontFullHeight + 1;
            Assert(YOffset + FontFullHeight + 1 < Atlas.Height);
        }

        r32 NormalizedXOffset = (r32)(XOffset) / Atlas.Width;
        r32 NormalizedYOffset = (r32)(YOffset) / Atlas.Height;
        r32 NormalizedGlyphWidth = (r32)(GlyphBitmapW) / (r32)Atlas.Width;
        r32 NormalizedGlyphHeight = (r32)(GlyphBitmapH) / (r32)Atlas.Height;

        Glyph->UV.TopLeft.x = NormalizedXOffset;
        Glyph->UV.TopLeft.y = NormalizedYOffset;

        Glyph->UV.TopRight.x = NormalizedXOffset + NormalizedGlyphWidth;
        Glyph->UV.TopRight.y = NormalizedYOffset;

        Glyph->UV.BottomLeft.x = NormalizedXOffset;
        Glyph->UV.BottomLeft.y = NormalizedYOffset + NormalizedGlyphHeight;

        Glyph->UV.BottomRight.x = NormalizedXOffset + NormalizedGlyphWidth;
        Glyph->UV.BottomRight.y = NormalizedYOffset + NormalizedGlyphHeight;

        u8 *Location = Atlas.Data + (YOffset * Atlas.Width) + XOffset;

        if (Glyph->Height && Glyph->Width) {
            u8 *Source = Face->glyph->bitmap.buffer;
            u8 *DestRow = Location;

            for (u32 y=0; y<GlyphBitmapH; ++y){
                u8 *Dest = DestRow;
                for (u32 x=0; x<GlyphBitmapW; ++x) {
                    *Dest++ = *Source++;
                }
                DestRow += Atlas.Width;
            }
        }

        XOffset += GlyphBitmapW + 1;
    }

    return Atlas;
}

cached_font
FreetypeLoadFont(char *Filename, font_ FontId, r32 SizePt, u8 RetinaScale, u32 AtlasW, u32 AtlasH)
{
    cached_font Font = {};
    Font.SizePt = SizePt;
    Font.Id = FontId;

    r32 SizePx = PointsToPixels(SizePt, SYSTEM_PPI) * RetinaScale;

    FT_Library FreeType;
    FT_Face Face;
    FT_Error Error = FT_Init_FreeType(&FreeType);

    Error = FT_New_Face(FreeType, Filename, 0, &Face);
    FT_Select_Charmap(Face, FT_ENCODING_UNICODE);

    sprintf(Font.Name, "%s", Face->family_name);

    u32 CodePointCount = Face->num_glyphs;
    Font.Advances = (r32 *)malloc(sizeof(r32) * CodePointCount * CodePointCount);
    Font.Map = (uincode_character_map *)malloc(sizeof(uincode_character_map) * CodePointCount);
    Font.Glyphs = (cached_glyph *)malloc(sizeof(cached_glyph) * CodePointCount);

    FT_ULong  CodePoint;
    u32 GlyphIndex;
    u32 FirstGlyphIndex;

    CodePoint = FT_Get_First_Char(Face, &GlyphIndex);
    while (GlyphIndex != 0) {
        uincode_character_map *MapEntry = Font.Map + GlyphIndex;
        MapEntry->Index = GlyphIndex;
        MapEntry->CodePoint = CodePoint;
        CodePoint = FT_Get_Next_Char(Face, CodePoint, &GlyphIndex);
    }

    Font.Map[0] = {};

    Font.PxPerFontUnit = SizePx / (Face->units_per_EM);
    // Font.MaxGlyphDim.x = Font.PxPerFontUnit * Face->max_advance_width;
    // Font.MaxGlyphDim.y = Font.PxPerFontUnit * (Face->ascender - Face->descender);
    Font.BaselineSpacing = Font.PxPerFontUnit * (Face->height);
    Font.Ascender = Font.PxPerFontUnit * (Face->ascender);
    Font.Descender = Font.PxPerFontUnit * (Face->descender) * -1.0f;
    Font.Baseline = Font.BaselineSpacing - Font.Descender;
    Font.GlyphCount = CodePointCount;

    u32 I = FT_Get_Char_Index(Face, 'T');
    I = FT_Get_Char_Index(Face, 'o');

    FT_Set_Char_Size(Face, 0, SizePt * 64, 96, 96);

    for (u32 i=0; i<CodePointCount; ++i) {
        for (u32 j=0; j<CodePointCount; ++j) {
            FT_Vector Kerning;
            FT_Get_Kerning(Face, i, j, FT_KERNING_UNSCALED, &Kerning);

            if (i==55 && j==82) {
                u32 a =4;
            }
            Font.Advances[i * Font.GlyphCount + j] = Kerning.x * Font.PxPerFontUnit;
        }
    }

    Font.Atlas = BakeFont(Font, Face, SYSTEM_PPI, SizePx, AtlasW, AtlasH);
    return Font;
}

s32
WriteFont(FILE *File, cached_font Font)
{
    packed_font Packed = {};

    memcpy(Packed.Name, Font.Name, 20);

    Packed.Id = Font.Id;
    Packed.GlyphCount = Font.GlyphCount;
    Packed.AtlasWidth = Font.Atlas.Width;
    Packed.AtlasHeight = Font.Atlas.Height;
    Packed.SizePt = Font.SizePt;
    Packed.PxPerFontUnit = Font.PxPerFontUnit;
    Packed.Height = Font.Height;
    Packed.Baseline = Font.Baseline;
    Packed.BaselineSpacing = Font.BaselineSpacing;
    Packed.Ascender = Font.Ascender;
    Packed.Descender = Font.Descender;

    s32 Size = 0;
    Size += fwrite(&Packed, 1, sizeof(Packed), File);
    Size += fwrite(Font.Map, 1, sizeof(uincode_character_map) * Font.GlyphCount, File);
    Size += fwrite(Font.Glyphs, 1, sizeof(cached_glyph) * Font.GlyphCount, File);
    Size += fwrite(Font.Advances, 1, sizeof(r32) * Font.GlyphCount * Font.GlyphCount, File);

    return Size;
}

void main()
{
    cached_font Fonts[20];
    u32 FontCount = 0;

    Fonts[FontCount++] = FreetypeLoadFont("fonts/PT_Sans.ttf", Font_PTSans, 10.0f, 1, 256, 128);
    Fonts[FontCount++] = FreetypeLoadFont("fonts/PT_Sans.ttf", Font_PTSans, 20.0f, 1, 512, 256);

    Fonts[FontCount++] = FreetypeLoadFont("fonts/PT_Sans.ttf", Font_PTSans, 30.0f, 1, 512, 512);
    Fonts[FontCount++] = FreetypeLoadFont("fonts/PT_Sans.ttf", Font_PTSans, 60.0f, 1, 1024, 1024);

    Fonts[FontCount++] = FreetypeLoadFont("fonts/PT_Sans.ttf", Font_PTSans, 100.0f, 1, 2048, 1024);
    Fonts[FontCount++] = FreetypeLoadFont("fonts/PT_Sans.ttf", Font_PTSans, 200.0f, 1, 4096, 2048);

    Fonts[FontCount++] = FreetypeLoadFont("fonts/PT_Sans.ttf", Font_PTSans, 400.0f, 1, 8192, 4096);

    FILE *File = fopen("fonts.data", "wb");

    file_header Header;
    sprintf(Header.M, "FONTS");
    Header.Count = FontCount;
    fwrite(&Header, 1, sizeof(file_header), File);

    for (u32 i=0; i<FontCount; ++i) {
        WriteFont(File, Fonts[i]);

        char Name[30];
        sprintf(Name, "%s_%d.png", Fonts[i].Name, (u32)Fonts[i].SizePt);
        stbi_write_png(Name, Fonts[i].Atlas.Width, Fonts[i].Atlas.Height, 1, Fonts[i].Atlas.Data, Fonts[i].Atlas.Width);
    }

    fclose(File);
}