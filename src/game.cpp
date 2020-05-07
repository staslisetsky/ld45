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
    // Station.MainModule.Origin = v2{125.0f, 125.0f};
    // Station.MainModule.Image = Image;
    // Station.MainModule.Interfaces[Side_Top].P = v2{0.0f, -120.0f};
    // Station.MainModule.Interfaces[Side_Right].P = v2{107.0f, 12.0f};
    // Station.MainModule.Interfaces[Side_Front].P = v2{-45.0f, 25.0f};

    // module *M = ModuleLibrary + Module_CylinderShort;
    // LoadImage("'art'/modules/cylinder_small/top.png", M->Images + Side_Top);
    // LoadImage("'art'/modules/cylinder_small/right.png", M->Images + Side_Right);
    // LoadImage("'art'/modules/cylinder_small/front.png", M->Images + Side_Front);

    // M->Origins[Side_Top] = v2{-25.0f, -82.0f};
    // M->Origins[Side_Right] = v2{-5.0f, -25.0f};
    // M->Origins[Side_Front] = v2{-50.0f, -25.0f};
}

void
Game(r32 dT)
{
    r32 Scale;
    cached_font *Font = FindMatchingFont(Font_PTSans, 20.0f, &Scale);

    // if (Input.Mouse[0].WentDown) {
    //     State.MoveTo = Input.MouseP;
    // }

    // Camera controls

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

    if (Input.dWheel != 0.0f) {
        Render.CameraScale += Input.dWheel / 1000.0f;
        Render.CameraScale = Clamp(0.5f, Render.CameraScale, 3.0f);
    }

    CamV += Normalize(FrameV) * CamSpeed;
    CamV = CamV * 0.9f;

    CamV.x = Clamp_r32(-CamSpeed, CamV.x, CamSpeed);
    CamV.y = Clamp_r32(-CamSpeed, CamV.y, CamSpeed);

    Render.CameraP += CamV * dT;

    //
    //
    //

    // State.Container = rect{v2{0.0f, 0.0f}, v2{500.0f, 300.0f}};
    // State.Soil = rect{v2{0.0f, 0.0f}, v2{100.0f, 100.0f}};

    DrawRect(v4{255.0f, 255.0f, 255.0f, 255.0f}, v2{0.0f, 0.0f}, {100.0f, 100.0f}, 0.0f);
    // DrawRect(v4{58.0f, 51.0f, 43.0f, 255.0f}, State.Soil.Min, State.Soil.Dim(), 2);

    // v2 WorldMouseP = Input.MouseP - v2{(r32)Render.Screen.x / 2.0f, (r32)Render.Screen.y / 2.0f};
    // WorldMouseP += Render.CameraP / Render.CameraScale;

    // v2 D = State.MoveTo - State.Dude.P;
    // if (Abs_r32(D.x) > 3.0f || Abs_r32(D.y) > 3.0f) {
    //     v2 V = Normalize(State.MoveTo - State.Dude.P) * 200.0f;
    //     State.Dude.P += V * dT;
    //     State.Dude.P.x = (s32)(State.Dude.P.x + 0.5f);
    //     State.Dude.P.y = (s32)(State.Dude.P.y + 0.5f);
    // } else {
    //     State.Dude.P = State.MoveTo;
    // }

    // u32 AnimationFrame = ((s32)(State.Time * 200.0f) % 256) / 16;

    // DrawImage(Station.MainModule.P - Station.MainModule.Origin, Station.MainModule.Image, 1.0f, 1);

    // for (u32 i=0; i<Side_Count; ++i) {
    //     if (i != (s32)Side_Top && i != (s32)Side_Right && i != (s32)Side_Front) {
    //         continue;
    //     }

    //     module *M = ModuleLibrary + Module_CylinderShort;

    //     v2 StationP = Station.MainModule.P - Station.MainModule.Origin;
    //     v2 P = Station.MainModule.P + Station.MainModule.Interfaces[i].P;

    //     rect Rect = RectCenterDim(P, 90.0f, 90.0f);
    //     v4 Color = RGBA(200,100,100,128);

    //     if (InRect(Rect, WorldMouseP)) {
    //         Color = RGBA(200,100,100,200);
    //         DrawImage(P + M->Origins[i], M->Images[i], 1.0f, 3);
    //     }

    //     // DrawRect(Color, Rect.Min, Rect.Dim(), 2);
    // }

    // DrawImage(Station.MainModule.P - Station.MainModule.Origin, Station.MainModule.Image, 1.0f, 1);
    // DrawImage(Station.MainModule.P, Station.Door[AnimationFrame], 1.0f, 2);

    // r32 Alpha = (sin(State.Time) + 1.0f) / 2.0f;
    // DrawRect(RGBA(255,10,10, Alpha * 255), v2{142.0f, 16.0f}, v2{1.0f,1.0f}, 2);
    // DrawRect(RGBA(255,200,200,255), State.MoveTo, v2{2.0f,2.0f}, 1);
    // DrawRect(RGBA(200,200,200,255), State.Dude.P, v2{10.0f,10.0f}, 2);

    // if (Font) {
    //     // DrawText(v2{50.0f, 50.0f}, 1, Scale, RGBA(200,100,100,255), Font, "Hello world", 11);
    // }

    State.Time += dT;
}