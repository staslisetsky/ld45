struct bullet {
    u32 Type;
    v2 P;
    v2 V;
};

struct asteroid {
    r32 Size;
    v2 P;
    v2 V;
};

enum game_mode_ {
    GameMode_Menu,
    GameMode_Play,
    GameMode_Lost,
};

struct state {
    game_mode_ GameMode;
    b32 GameStarted;
    u32 FrameCounter;

    v2 PlayerP;
    v2 PlayerV;
    v2 PlayerFacing;

    r32 Time;

    std::vector<bullet>Bullets;
    std::vector<asteroid>Asteroids;
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
SpawnAsteroids(u32 Count, u32 SizeFactor)
{
    r32 SizeBase = 50.0f;
    r32 MaxSpeed = 290.0f;

    for (u32 i=0; i<Count; ++i) {
        r32 Rand = (r32)(rand() % 100);
        r32 RandX = (r32)(rand() % 200) - 100.0f;
        r32 RandY = (r32)(rand() % 200) - 100.0f;
        r32 RandSpeed = (r32)(rand() % 100) / 100.0f;

        if (RandX > 0.0) {
            RandX += Render.Screen.x - 200.0;
        } else {
            RandX += 200.0;
        }
        if (RandY > 0.0) {
            RandY += Render.Screen.y - 100.0;
        } else{
            RandY += 100.0;
        }

        asteroid New = {};
        New.Size = SizeBase + SizeFactor * Rand;
        New.P = v2{RandX, RandY};

        v2 Center = v2{Render.Screen.x / 2.0f, Render.Screen.y / 2.0f};
        v2 ToCenter = Center - New.P;
        v2 N = Normalize(Perp(ToCenter));

        r32 V1 = Dot(v2{0.0f, 0.0f} - Center, N);
        r32 V2 = Dot(v2{(r32)Render.Screen.x, 0.0f} - Center, N);
        r32 V3 = Dot(v2{(r32)Render.Screen.x, (r32)Render.Screen.y} - Center, N);
        r32 V4 = Dot(v2{0.0f, (r32)Render.Screen.y} - Center, N);

        r32 RandDirection = (r32)(rand() % 100) / 100.0f;

        v2 MinDirection = Normalize(Min_r32(Min_r32(V1, V2), Min_r32(V3, V4)) * N + Center - New.P);
        v2 MaxDirection = Normalize(Max_r32(Max_r32(V1, V2), Max_r32(V3, V4)) * N + Center - New.P);

        v2 Direction = Lerp(MinDirection, MaxDirection, RandDirection);

        New.V = Direction * (Max_r32(RandSpeed * MaxSpeed, 70.0f));

        State.Asteroids.push_back(New);
    }
}

void
Game(r32 dT)
{
    if (State.GameMode == GameMode_Menu) {
        if (!State.GameStarted) {
            sound::Play(Music_Intro);
            State.GameStarted = true;
        }

        r32 Size = ((sin(State.Time) + 1.0f) / 2.0f) * 10.0f + 30.0f;
        r32 Color = ((sin(State.Time) + 1.0f) / 2.0f) * 200.0f + 55;

        Layout.FontSpacing = 0.3f;

        v2 TextDim = GetTextDim(Font_PTSansCaption, Size, "MEGA SPACE FUCK");
        v2 TextP = v2{Render.Screen.x / 2.0f - TextDim.x / 2.0f, 100.0f};
        DrawText(&Render, TextP, RGBA(Color,0,0,255), Font_PTSansCaption, Size, "MEGA SPACE FUCK");

        Layout.FontSpacing = 0.0f;

        TextDim = GetTextDim(Font_PTSansCaption, Size * 2.5, "3");
        TextP = v2{Render.Screen.x / 2.0f - TextDim.x / 2.0f, 180.0f};
        DrawText(&Render, TextP, RGBA(Color,0,0,255), Font_PTSansCaption, Size * 2.5, "3");

        TextDim = GetTextDim(Font_PTSans, 18.0f, "Insert Coin");
        TextP = v2{Render.Screen.x / 2.0f - TextDim.x / 2.0f, Render.Screen.y - TextDim.y - 50.0f};
        DrawText(&Render, TextP, RGBA(186,186,186,255), Font_PTSans, 18.0, "Insert Coin");

        if (Input.Keys[Key_Enter].WentDown) {
            State.GameMode = GameMode_Play;
        }
    } else {
        sound::Loop(Music_Main);

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

            // sound::Play(Sound_Pew);
        }

        State.PlayerFacing = Normalize(Input.MouseP - State.PlayerP);

        Direction = NormalizeZero(Direction);

        v2 dV = (Acceleration * Direction) * dT;
        v2 dP = State.PlayerV + dV * dT;

        State.PlayerV += dV;
        State.PlayerP += dP;

        if (State.Time == 0) {
            SpawnAsteroids(3, 1);
        }

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
        if (State.PlayerP.x > Render.Screen.x - PlayerSize) {
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
            DrawRect(&Render, RGBA(255,0,0,255), State.Bullets[i].P, v2{7.0, 7.0f}, 1);

            for (u32 j=0; j<State.Asteroids.size(); ++j) {
                rect Rect = RectPDim(State.Asteroids[j].P, v2{State.Asteroids[j].Size, State.Asteroids[j].Size});

                if (InRect(Rect, State.Bullets[i].P)) {
                    State.Bullets.erase(State.Bullets.begin() + i);
                    State.Asteroids.erase(State.Asteroids.begin() + j);

                    if (i > 0) {
                        --i;
                    }
                    if (j > 0) {
                        --j;
                    }

                    sound::Play(Sound_AsteroidExplode);
                }
            }
        }

        for (u32 i=0; i<State.Asteroids.size(); ++i) {
            v2 dP = State.Asteroids[i].V * dT;
            State.Asteroids[i].P += dP;
            r32 Size = State.Asteroids[i].Size;
            DrawRect(&Render, RGBA(255,0,0,255), State.Asteroids[i].P, v2{Size, Size}, 1);
        }
    }


    State.Time += dT;
}
