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
#include "layout.h"
#include "font.h"

#define SYSTEM_PPI 96.0

r32
PointsToPixels(r32 Points, r32 PPI)
{
    r32 Result = (Points / 72.0f) * SYSTEM_PPI;
    //                  in

    return(Result);
}

r32
PixelsToPoints(r32 Pixels, r32 PPI)
{
    r32 Result =  Pixels / PPI * SYSTEM_PPI;
    return(Result);
}

/* oversampled + prefiltered technique
   credit: stb_image.h lib by Sean Barrett
   https://github.com/nothings/stb */

void
FontPrefilterH(u8 *Bitmap, u32 W, u32 H, u32 StrideBytes, u32 KernelWidth)
{
    if (KernelWidth <= 1) { return; }

    u32 MaxOversample = 8;
    u32 OversampleMask = MaxOversample - 1;

    u8 Buffer[8] = {};
    u32 SafeW = W - KernelWidth;
    u32 j;

    for (j=0; j < H; ++j) {
        u32 i;
        u32 Total;
        memset(Buffer, 0, MaxOversample);

        Total = 0;

      // make KernelWidth a constant in common cases so compiler can optimize out the divide
        switch (KernelWidth) {
            case 2:
            for (i=0; i <= SafeW; ++i) {
                Total += Bitmap[i] - Buffer[i & OversampleMask];
                Buffer[(i+KernelWidth) & OversampleMask] = Bitmap[i];
                Bitmap[i] = (u8) (Total / 2);
            }
            break;
            case 3:
            for (i=0; i <= SafeW; ++i) {
                Total += Bitmap[i] - Buffer[i & OversampleMask];
                Buffer[(i+KernelWidth) & OversampleMask] = Bitmap[i];
                Bitmap[i] = (u8) (Total / 3);
            }
            break;
            case 4:
            for (i=0; i <= SafeW; ++i) {
                Total += Bitmap[i] - Buffer[i & OversampleMask];
                Buffer[(i+KernelWidth) & OversampleMask] = Bitmap[i];
                Bitmap[i] = (u8) (Total / 4);
            }
            break;
            case 5:
            for (i=0; i <= SafeW; ++i) {
                Total += Bitmap[i] - Buffer[i & OversampleMask];
                Buffer[(i+KernelWidth) & OversampleMask] = Bitmap[i];
                Bitmap[i] = (u8) (Total / 5);
            }
            break;
            default:
            for (i=0; i <= SafeW; ++i) {
                Total += Bitmap[i] - Buffer[i & OversampleMask];
                Buffer[(i+KernelWidth) & OversampleMask] = Bitmap[i];
                Bitmap[i] = (u8) (Total / KernelWidth);
            }
            break;
        }

        for (; i < W; ++i) {
            // Assert(Bitmap[i] == 0);
            Total -= Buffer[i & OversampleMask];
            Bitmap[i] = (u8) (Total / KernelWidth);
        }

        Bitmap += StrideBytes;
    }
}

bitmap
BakeFont(cached_font Font, FT_Face Face, u32 PPI, r32 SizePx, u32 W, u32 H)
{
    bitmap Atlas = {};
    Atlas.Width = W;
    Atlas.Height = H;
    Atlas.Data = (u8 *)calloc(W * H, 1);

    r32 FontFullHeight = Face->height * Font.PxPerFontUnit;

    u32 XOffset = 3;
    u32 YOffset = 3;

    u32 OversampleX = 1;

    if (SizePx <= 20) {
        OversampleX = 3;
    }
    if (SizePx < 14) {
        OversampleX = 5;
    }
    if (SizePx < 10) {
        OversampleX = 7;
    }

    for (u32 i = 0; i < Font.GlyphCount; ++i) {
        uincode_character_map *CharMapEntry = Font.Map + i;
        if (CharMapEntry->CodePoint > 0xb0) {
            break;
        }
        FT_Set_Char_Size(Face, 0, PixelsToPoints(SizePx, SYSTEM_PPI) * 64, PPI * OversampleX, PPI);
        FT_Load_Glyph(Face, CharMapEntry->Index, FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT);

        FT_Glyph_Metrics Metrics = Face->glyph->metrics;

        cached_glyph *Glyph = Font.Glyphs + i;
        *Glyph = {};

        Glyph->CodePoint = CharMapEntry->CodePoint;
        Glyph->XAdvance = (r32)Metrics.horiAdvance / 64.0f / (r32)OversampleX;
        Glyph->Width  = (r32)Metrics.width / 64.0f / (r32)OversampleX;
        Glyph->Height = (r32)Metrics.height / 64.0f;
        Glyph->LeftBearing = (r32)Metrics.horiBearingX / 64.0 / (r32)OversampleX;
        Glyph->TopBearing = (r32)Metrics.horiBearingY / 64.0;

        //
        //
        //

        FT_Render_Glyph(Face->glyph, FT_RENDER_MODE_NORMAL);
        Glyph->BitmapTop = Face->glyph->bitmap_top;
        Glyph->BitmapLeft = Face->glyph->bitmap_left;
        Glyph->BitmapWidth = Face->glyph->bitmap.width;
        Glyph->BitmapHeight = Face->glyph->bitmap.rows;

        if (XOffset + Glyph->BitmapWidth + (OversampleX * 2) >= Atlas.Width - 3) {
            XOffset = 3;
            YOffset += FontFullHeight + 3;
            Assert(YOffset + FontFullHeight + 3 < Atlas.Height);
        }

        r32 NormalizedXOffset = (r32)(XOffset) / Atlas.Width;
        r32 NormalizedYOffset = (r32)(YOffset) / Atlas.Height;
        r32 NormalizedGlyphWidth = (r32)(Glyph->BitmapWidth + OversampleX * 2.0f) / (r32)Atlas.Width;
        r32 NormalizedGlyphHeight = (r32)(Glyph->BitmapHeight) / (r32)Atlas.Height;

        Glyph->UV.TopLeft.x = NormalizedXOffset;
        Glyph->UV.TopLeft.y = NormalizedYOffset;

        Glyph->UV.TopRight.x = NormalizedXOffset + NormalizedGlyphWidth;
        Glyph->UV.TopRight.y = NormalizedYOffset;

        Glyph->UV.BottomLeft.x = NormalizedXOffset;
        Glyph->UV.BottomLeft.y = NormalizedYOffset + NormalizedGlyphHeight;

        Glyph->UV.BottomRight.x = NormalizedXOffset + NormalizedGlyphWidth;
        Glyph->UV.BottomRight.y = NormalizedYOffset + NormalizedGlyphHeight;

        u8 *Location = Atlas.Data + (YOffset * Atlas.Width) + XOffset + OversampleX;

        if (Glyph->Height && Glyph->Width) {
            u8 *Source = Face->glyph->bitmap.buffer;
            u8 *DestRow = Location;

            for (u32 y=0; y<Face->glyph->bitmap.rows; ++y){
                u8 *Dest = DestRow;
                for (u32 x=0; x<Face->glyph->bitmap.width; ++x) {
                    *Dest++ = *Source++;
                }
                DestRow += Atlas.Width;
            }
        }

        Location = (u8 *)Atlas.Data + (YOffset * Atlas.Width) + XOffset;
        FontPrefilterH(Location, Glyph->BitmapWidth + (OversampleX * 2), Glyph->BitmapHeight, Atlas.Width, OversampleX);

        // note: BitmapWidth is used by the game to determine the Quad with for the glyph, so it has to be scaled

        XOffset += Glyph->BitmapWidth + (OversampleX * 2) + 3;

        if (OversampleX > 1) {
            Glyph->BitmapWidth = (u32)ceil((r32)(Glyph->BitmapWidth) / (r32)OversampleX);
        }
    }

    return Atlas;
}

cached_font
FreetypeLoadFont(char *Filename, font_ FontId, r32 SizePx, u32 AtlasW, u32 AtlasH)
{
    cached_font Font = {};
    Font.SizePx = SizePx;
    Font.Id = FontId;

    FT_Library FreeType;
    FT_Face Face;
    FT_Error Error = FT_Init_FreeType(&FreeType);

    Error = FT_New_Face(FreeType, Filename, 0, &Face);
    FT_Select_Charmap(Face, FT_ENCODING_UNICODE);

    sprintf(Font.Name, "%s", Face->family_name);

    u32 CodePointCount = Face->num_glyphs;
    Font.Advances = (r32 *)malloc(sizeof(r32) * CodePointCount * CodePointCount);

    Font.Map = (uincode_character_map *)malloc(sizeof(uincode_character_map) * CodePointCount);
    Font.Map[0] = {};

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

    FT_Set_Char_Size(Face, 0, SizePx * 64, 96, 96);

    for (u32 i=0; i<CodePointCount; ++i) {
        for (u32 j=0; j<CodePointCount; ++j) {
            FT_Vector Kerning;
            FT_Get_Kerning(Face, i, j, FT_KERNING_UNSCALED, &Kerning);
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
    Packed.SizePx = Font.SizePx;
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
    cached_font Fonts[50];
    u32 FontCount = 0;

    Fonts[FontCount++] = FreetypeLoadFont("fonts/PT_Sans.ttf", Font_PTSans, 8.0f, 512, 256);
    Fonts[FontCount++] = FreetypeLoadFont("fonts/PT_Sans.ttf", Font_PTSans, 12.0f, 512, 256);
    Fonts[FontCount++] = FreetypeLoadFont("fonts/PT_Sans.ttf", Font_PTSans, 20.0f, 512, 512);
    Fonts[FontCount++] = FreetypeLoadFont("fonts/PT_Sans.ttf", Font_PTSans, 40.0f, 512, 512);
    Fonts[FontCount++] = FreetypeLoadFont("fonts/PT_Sans.ttf", Font_PTSans, 60.0f, 1024, 512);

    FILE *File = fopen("fonts.data", "wb");

    file_header Header;
    sprintf(Header.M, "FONTS");
    Header.Count = FontCount;
    fwrite(&Header, 1, sizeof(file_header), File);

    for (u32 i=0; i<FontCount; ++i) {
        WriteFont(File, Fonts[i]);

        char Name[30];
        sprintf(Name, "%s_%d.png", Fonts[i].Name, (u32)Fonts[i].SizePx);
        stbi_write_png(Name, Fonts[i].Atlas.Width, Fonts[i].Atlas.Height, 1, Fonts[i].Atlas.Data, Fonts[i].Atlas.Width);
    }

    fclose(File);
}