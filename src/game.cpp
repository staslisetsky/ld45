#include "game.h"
#include "layout.cpp"

void
Cleanup(image Image)
{
    u32 Pitch = Image.N * Image.Width;
    for (u32 y=0; y<Image.Height; ++y) {
        for (u32 x=0; x<Image.Width; ++x) {
            u8 *Pixel = Image.Data + y * Pitch + x * Image.N;
            if (Pixel[0] == 0xff || Pixel[1] == 0xff || Pixel[2] == 0xff) {
                Pixel[0] = 0;
                Pixel[1] = 0;
                Pixel[2] = 0;
                Pixel[3] = 0;
            }
        }
    }
}

void
LoadImage(char *Path, image *Image)
{
    Image->Data = stbi_load(Path, (s32 *)&Image->Width, (s32 *)&Image->Height, (s32 *)&Image->N, 0);
    Image->Texture = OpenglUploadTexture(*Image);
}

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
        Font->SizePx = Packed->SizePx;
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
        sprintf(Name, "%s_%d.png", Font->Name, (u32)Font->SizePx);

        image Image = {};
        Image.Data = stbi_load(Name, (s32 *)&Image.Width, (s32 *)&Image.Height, (s32 *)&Image.N, 0);
        Font->Atlas.Texture = OpenglUploadTexture(Image);

        ++CachedFontCount;
    }

    // load texture
    // image Image = {};
    // Image.Data = stbi_load("'art'/main_module.png", (s32 *)&Image.Width, (s32 *)&Image.Height, (s32 *)&Image.N, 0);
    // Image.Texture = OpenglUploadTexture(Image);
}

void
DoGameFrame(r32 dT)
{
    r32 Scale;
    cached_font *Font = FindMatchingFont(Font_PTSans, 20.0f, &Scale);

    r32 CamSpeed = 500.0f;
    static v2 CamV = {};

    v2 FrameV = {};
    if (Input.Keys[Key_W].Down) {
        FrameV.y += 1.0f;
    }
    if (Input.Keys[Key_S].Down) {
        FrameV.y -= 1.0f;
    }
    if (Input.Keys[Key_A].Down) {
        FrameV.x -= 1.0f;
    }
    if (Input.Keys[Key_D].Down) {
        FrameV.x += 1.0f;
    }

    if (Input.MouseWheel != 0.0f) {
        Game.CameraScale += Input.MouseWheel / 1000.0f;
        Game.CameraScale = Clamp(0.5f, Game.CameraScale, 3.0f);
    }

    CamV += Normalize(FrameV) * CamSpeed;
    CamV = CamV * 0.9f;

    CamV.x = Clamp_r32(-CamSpeed, CamV.x, CamSpeed);
    CamV.y = Clamp_r32(-CamSpeed, CamV.y, CamSpeed);

    Game.CameraP += CamV * dT;

    //
    //

    // DrawText(v2{0.0f, 0.0f}, v4{100.0f, 200.0f, 100.0f, 255.0f}, Font_PTSans, 30.0f, "Success!", 8);

    Game.Container = rect{v2{0.0f, 0.0f}, v2{500.0f, 300.0f}};
    Game.Soil = rect{v2{0.0f, 0.0f}, v2{500.0f, 50.0f}};

    Renderer.SetMatrix(Game.CameraP, true, true, 1.0f);

    DrawRect(v4{10.0f,10.0f,10.0f,255.0f}, Game.Container.Min, Game.Container.Dim(), 0.0f);
    DrawRect(v4{28.0f, 21.0f, 33.0f, 255.0f}, Game.Soil.Min, Game.Soil.Dim(), 2);

    Renderer.Flush();

    // v2 WorldMouseP = Input.MouseP - v2{(r32)Render.Screen.x / 2.0f, (r32)Render.Screen.y / 2.0f};
    // WorldMouseP += Render.CameraP / Render.CameraScale;

    Game.Time += dT;
}