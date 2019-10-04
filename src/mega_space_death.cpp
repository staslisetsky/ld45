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

    b32 Wave;
};

state State = {};

void
SpawnAsteroids(u32 Count)
{
    r32 SizeBase = 15.0f;
    r32 MaxSpeed = 290.0f;

    for (u32 i=0; i<Count; ++i) {
        r32 Rand = (r32)(rand() % 100) / 100.0f;
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
        New.Size = SizeBase + 50.0f * Rand;
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

        Layout.FontSpacing = 0.3f;

        v2 TextDim = GetTextDim(Font_PTSansCaption, Size, "MEGA SPACE DEATH");
        v2 TextP = v2{Render.Screen.x / 2.0f - TextDim.x / 2.0f, 100.0f};
        DrawText(&Render, TextP, RGBA(255,0,0,255), Font_PTSansCaption, Size, "MEGA SPACE DEATH");

        Layout.FontSpacing = 0.0f;

        TextDim = GetTextDim(Font_PTSansCaption, Size * 2.5, "3");
        TextP = v2{Render.Screen.x / 2.0f - TextDim.x / 2.0f, 180.0f};
        DrawText(&Render, TextP, RGBA(255,0,0,255), Font_PTSansCaption, Size * 2.5, "3");

        TextDim = GetTextDim(Font_PTSans, 16.0f, "Insert Coin");
        TextP = v2{Render.Screen.x / 2.0f - TextDim.x / 2.0f, Render.Screen.y - TextDim.y - 65.0f};
        DrawText(&Render, TextP, RGBA(186,186,186,255), Font_PTSans, 16.0f, "Insert Coin");

        TextDim = GetTextDim(Font_PTSans, 8.0f, "or press SPACE, I dont't care");
        TextP = v2{Render.Screen.x / 2.0f - TextDim.x / 2.0f, Render.Screen.y - TextDim.y - 35.0f};
        DrawText(&Render, TextP, RGBA(186,186,186,255), Font_PTSans, 8.0f, "or press SPACE, I dont't care");

        if (Input.Keys[Key_Space].WentDown) {
            State.GameMode = GameMode_Play;
            State.Time = 0.0f;
        }
    }

    if (State.GameMode == GameMode_Play) {
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

            sound::Play(Sound_Pew);
        }

        State.PlayerFacing = Normalize(Input.MouseP - State.PlayerP);

        Direction = NormalizeZero(Direction);

        v2 dV = (Acceleration * Direction) * dT;
        v2 dP = State.PlayerV + dV * dT;

        State.PlayerV += dV;
        State.PlayerP += dP;

        if (State.Wave == 0 && State.Time > 1) {
            SpawnAsteroids(3);
            State.Wave = 1;
        } else if (State.Wave == 1 && State.Time > 5) {
            SpawnAsteroids(4);
            State.Wave = 2;
        } else if (State.Wave == 2 && State.Time > 10) {
            SpawnAsteroids(5);
            State.Wave = 3;
        } else if (State.Wave == 3 && State.Time > 12) {
            SpawnAsteroids(6);
            State.Wave = 4;
        } else if (State.Wave == 4 && State.Time > 15) {
            SpawnAsteroids(7);
            State.Wave = 5;
        } else if (State.Wave == 5 && State.Time > 18) {
            SpawnAsteroids(8);
            State.Wave = 6;
        } else if (State.Wave == 6 && State.Time > 22) {
            SpawnAsteroids(8);
            State.Wave = 7;
        } else if (State.Wave == 7 && State.Time > 28) {
            SpawnAsteroids(8);
            State.Wave = 8;
        } else if (State.Wave == 8 && State.Time > 30) {
            SpawnAsteroids(8);
            State.Wave = 9;
        } else if (State.Wave == 9 && State.Time > 35) {
            SpawnAsteroids(8);
            State.Wave = 10;
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