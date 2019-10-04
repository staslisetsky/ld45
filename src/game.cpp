struct state {
};

state State = {};

void
GameInit()
{
    read_file FontFile;
    os::ReadFile("fonts.data", &FontFile);

    u8 *At = FontFile.Data;

    file_header Header = *(file_header *)At;
    u32 HeaderSize = sizeof(file_header);
    At += HeaderSize;

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

        ++CachedFontCount;
    }
}


void
Game(r32 dT)
{
}