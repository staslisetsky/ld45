void
GameInit()
{
    read_file FontFile;
    PlatformReadFile("fonts.data", &FontFile);

    u8 *At = FontFile.Data;

    file_header Header = *(file_header *)At;
    u32 HeaderSize = sizeof(file_header);
    At += HeaderSize;

    // WasmConsoleLog(F->Name);
    // EM_ASM(console.log('Header size: ' + $0), Size);

    for (u32 i=0; i<Header.Count; ++i) {
        cached_font *Font = FontCache + CachedFontCount;
        packed_font *Packed = (packed_font *)At;

        memcpy(Font->Name, Packed->Name, 20);
        Font->Id = Packed->Id;
        Font->GlyphCount = Packed->GlyphCount;
        Font->Atlas.Width = Packed->AtlasWidth;
        Font->Atlas.Height = Packed->AtlasHeight;
        Font->SizePt = Packed->SizePt;
        Font->PxPerFontUnit = Packed->PxPerFontUnit;
        Font->Height = Packed->Height;
        Font->Baseline = Packed->Baseline;
        Font->BaselineSpacing = Packed->BaselineSpacing;
        Font->Ascender = Packed->Ascender;
        Font->Descender = Packed->Descender;

        At += sizeof(packed_font);

        Font->Map = (uincode_character_map *)At;

        // EM_ASM(console.log('Count ' + $0), Font->GlyphCount);
        At += sizeof(uincode_character_map) * Font->GlyphCount;

        Font->Glyphs = (cached_glyph *)At;
        At += sizeof(cached_glyph) * Font->GlyphCount;

        Font->Advances = (r32 *)At;
        At += sizeof(r32) * Font->GlyphCount * Font->GlyphCount;

        char Name[30];
        sprintf(Name, "%s_%d.png", Font->Name, (u32)Font->SizePt);

        image Image = {};
        Image.Data = stbi_load(Name, (s32 *)&Image.Width, (s32 *)&Image.Height, (s32 *)&Image.N, 0);
        Font->Atlas.Texture = OpenglUploadTexture(Image);

        // WasmConsoleLog(Name);
        // EM_ASM(console.log('Texture ' + $0), Font->Atlas.Texture);

        ++CachedFontCount;
    }

    // WasmConsoleLog("Font files loaded");
}

void
Game(r32 dT)
{
    static u32 FrameCounter = 0;
    r32 Z = (sin((r32)FrameCounter / 100.0f) + 1.0f) / 2.0f * 200.0f + 20.0f;
    // static r32 Y = 0.0f;
    DrawText(&Render, v2{100, 100}, v4{0.0, 0.0, 0.0, 1.0f}, Font_PTSans, Z, "To Playing jazz vibe");
    ++FrameCounter;
    // Y += 0.01;
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
