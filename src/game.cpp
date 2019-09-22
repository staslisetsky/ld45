// enum font_ {
//     Font_Default,
//     Font_PTSans,
// };

// enum font_style_ {
//     FontStyle_Normal,
//     FontStyle_Bold,
//     FontStyle_Italic,
// };

// struct font {
//     font_ Font;
//     font_style_ Style;
//     r32 SizePt;
//     r32 SizePx;

//     glyph *Glyphs;
//     u32 GlyphCount;
// };

// static font Fonts[10] = {};
// static u32 FontCount;
static cached_font *FontCache[20];
static u32 CachedFontCount = 0;

void
GameInit()
{
    read_file FontFile;
    WindowsReadFile("fonts.data", &FontFile);

    u8 *At = FontFile.Data;

    file_header Header = *(file_header *)At;
    At += sizeof(file_header);

    for (u32 i=0; i<Header.Count; ++i) {
        cached_font **Font = FontCache + CachedFontCount;
        *Font = (cached_font *)At;
        At += sizeof(cached_font);

        cached_font *F = FontCache[CachedFontCount];
        F->Map = (uincode_character_map *)At;
        At += sizeof(uincode_character_map) * F->GlyphCount;

        F->Glyphs = (cached_glyph *)At;
        At += sizeof(cached_glyph) * F->GlyphCount;

        F->Advances = (r32 *)At;
        At += sizeof(r32) * F->GlyphCount;

        char Name[30];
        sprintf(Name, "%s_%d.png", F->Name, (u32)F->SizePt);

        image Image = {};
        Image.Data = stbi_load(Name, (s32 *)&Image.Width, (s32 *)&Image.Height, (s32 *)&Image.N, 0);
        Render.TestTexture = OpenglUploadTexture(Image);
    }
}

void
Game(r32 dT)
{
    static u32 FrameCounter = 0;

    r32 Z = (sin((r32)FrameCounter / 60.0f / 2.0f) + 1.0f) / 2.0f * 200.0f + 100.0f;

    cached_font *Font = FontCache[0];
    // DrawGlyph(&Render, v2{100, 100}, Z, v4{1.0, 0.0, 0.0, 1.0f}, Font_PTSans, 0, 13.5. "Hello world, this is Patrick!");
    DrawText(&Render, v2{100, 100}, Z, v4{1.0, 0.0, 0.0, 1.0f}, Font, "Hello!");

    ++FrameCounter;
}

// tool -font PT_Sans -size 16
// png
// .font
// 0. scale, baseline
// 1. kerning
// 2. glyph
//     - size
//     - advance
//     - UV
