struct bullet {
    u32 Type;
    v2 P;
    v2 V;
};

struct state {
    v2 PlayerP;
    v2 PlayerV;
    v2 PlayerFacing;

    std::vector<bullet>Bullets;
};

state State = {};

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
    r32 Acceleration = 5.0f;
    v2 Direction = {};

    if (Input.Keys[Key_D].Down) {
        Direction += v2{1.0f, 0.0f};
    }
    if (Input.Keys[Key_A].Down) {
        Direction += v2{-1.0f, 0.0f};
    }
    if (Input.Keys[Key_W].Down) {
        Direction += v2{0.0f, -1.0f};
    }
    if (Input.Keys[Key_S].Down) {
        Direction += v2{0.0f, 1.0f};
    }

    if (Input.Mouse[0].WentDown) {
        bullet Bullet;
        Bullet.P = State.PlayerP;
        Bullet.V = State.PlayerFacing * 1000.0f;

        State.Bullets.push_back(Bullet);

        sound::Play();
    }

    State.PlayerFacing = Normalize(Input.MouseP - State.PlayerP);

    Direction = NormalizeZero(Direction);

    v2 dV = (Acceleration * Direction) * dT;
    v2 dP = State.PlayerV + dV * dT;

    State.PlayerV += dV;
    State.PlayerP += dP;

    //
    //
    //

    r32 PlayerSize = 40.0f;

    DrawPlayer(&Render, RGBA(255,255,255,255), State.PlayerP, Input.MouseP, 50.0f, 1);

    if (State.PlayerP.x > PlayerSize + Render.Screen.x) {
        State.PlayerP.x -= Render.Screen.x;
    }
    if (State.PlayerP.x < -PlayerSize) {
        State.PlayerP.x += Render.Screen.x;
    }
    if (State.PlayerP.y > PlayerSize + Render.Screen.y) {
        State.PlayerP.y -= Render.Screen.y;
    }
    if (State.PlayerP.y < -PlayerSize) {
        State.PlayerP.y += Render.Screen.y;
    }

    if (State.PlayerP.x < PlayerSize) {
        v2 Offset = v2{(r32)Render.Screen.x, 0.0f};
        DrawPlayer(&Render, RGBA(255,255,255,255), State.PlayerP + Offset, Input.MouseP + Offset, 50.0f, 1);
    }
    if (State.PlayerP.x < Render.Screen.x - PlayerSize) {
        v2 Offset = v2{(r32)-Render.Screen.x, 0.0f};
        DrawPlayer(&Render, RGBA(255,255,255,255), State.PlayerP + Offset, Input.MouseP + Offset, 50.0f, 1);
    }
    if (State.PlayerP.y < PlayerSize) {
        v2 Offset = v2{0.0f, (r32)Render.Screen.y};
        DrawPlayer(&Render, RGBA(255,255,255,255), State.PlayerP + Offset, Input.MouseP + Offset, 50.0f, 1);
    }
    if (State.PlayerP.y > Render.Screen.y - PlayerSize) {
        v2 Offset = v2{0.0f, (r32)-Render.Screen.y};
        DrawPlayer(&Render, RGBA(255,255,255,255), State.PlayerP + Offset, Input.MouseP + Offset, 50.0f, 1);
    }

    for (u32 i=0; i<State.Bullets.size(); ++i) {
        if (State.Bullets[i].P.x < -100 || State.Bullets[i].P.x > Render.Screen.x + 100.0f ||
            State.Bullets[i].P.y < -100 || State.Bullets[i].P.y > Render.Screen.y + 100.0f)
        {
            State.Bullets.erase(State.Bullets.begin() + i);
            --i;
        }
    }

    for (u32 i=0; i<State.Bullets.size(); ++i) {
        v2 dP = State.Bullets[i].V * dT;
        State.Bullets[i].P += dP;
        DrawRect(&Render, RGBA(255,0,0,255), State.Bullets[i].P, v2{20.0, 20.0f}, 1);
    }
}
