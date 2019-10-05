#include "game.h"
#include "layout.cpp"
#include "commands.cpp"

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

    State.SecondsPerGlyph = 1.0 / 42.0f;

    SetScene(Scene_Intro);

    // CommandSimpleText(Text_Normal, 0.4f, 1.5f, 1.0f, "Hey");
    // CommandSimpleText(Text_Normal, 0.4f, 1.5f, 1.0f, "I know you're there");

    CommandTextLayout(Text_Normal, "So uh[p:0.8], do you want to ");
    auto *Command = CommandTextLayout(Text_Play, "play");
    Command->Scene = Scene_Start;
    CommandTextLayout(Text_Normal, " a game?");

    CommandPause(1.0f);

    CommandTextLayout(Text_Normal, "\nOr[p:0.1], I mean[p:0.5], you could always say ");
    Command = CommandTextLayout(Text_No, "no");
    Command->Scene = Scene_Quit;
    CommandTextLayout(Text_Normal, ".");
    CommandPause(1.2f);
    CommandTextLayout(Text_Normal, "\n\nI'm not, like, forcing you or anything.");

    SetScene(Scene_Quit);

    CommandTextLayout(Text_Normal, "Uh...[p:0.7] ok then. [p:1.0]Your call.");
    CommandPause(0.6f);
    CommandTextLayout(Text_Normal, "\n\nThe End.[p:2.0] Or whatever.[p:1.0] Bye.");

    SetScene(Scene_Start);
    CommandTextLayout(Text_Normal, "You find yourself in the center of a large room. In front of you there's a large stone statue. And there's two doors.");
    // CommandTextLayout(Text_Door, "The first door ");
    // CommandTextLayout(Text_Normal, "and ");
    // CommandTextLayout(Text_Door, "the second door");
    // CommandTextLayout(Text_Normal, ".");

    SetScene(Scene_Intro);

    // Scene(Scene_MonsterRoom1);
    // CommandTextLayout(Text_Normal, "This is the monster room. Go back to the ");
    // CommandTextLayout(Text_Door, "main room");
    // CommandTextLayout(Text_Normal, ".");

    // Scene(Scene_StorageRoom1);
    // CommandTextLayout(Text_Normal, "This is the storage room. Go back to the ");
    // CommandTextLayout(Text_Door, "main room");
    // CommandTextLayout(Text_Normal, ".");

    // CommandTextLayout(Text_Normal,
    // "So you walk into the room, right?[p:0.5] "
    // "And, like[p:0.09], there's this [s:0.4]giant Monster[s:1.0, p:0.3] just kinda standing right there.\n\n"
    // "[p:0.7,s:0.7]And he totally[p:0.8] fucking[p:0.7,s:0.8] sees you."
    // "Pretty much the only option you have[p:0.3] is to "
    // );
    // CommandTextLayout(Text_Panic, "run and scream");
    // CommandTextLayout(Text_Normal, ".");
    // Scene(Scene_MonsterRoom2);
    // CommandTextLayout(Text_Normal,
    // "As you're about to start running, the Monster politely greets you."
    // "\"Hello there Sir.[p:0.7] Can I help you?\""
    // );
    // CommandTextLayout(Text_Normal, "You enter ");
    // CommandTextLayout(Text_Door, "The first door ");
    // CommandTextLayout(Text_Normal, "and ");
    // CommandTextLayout(Text_Door, "the second door");
    // CommandTextLayout(Text_Normal, ".");
}

void
Game(r32 dT)
{
    State.Layout.Min = v2{50.0f, 40.0f};
    State.Layout.Max = v2{(r32)Render.Screen.x - 100.0f, (r32)Render.Screen.y - 80.0f};
    State.P = State.Layout.Min;
    State.Hovered = {};

    for (u32 i=0; i<State.InteractiveCount; ++i) {
        ui_id UIID = State.Interactive[i];
        if (InRect(UIID.Rect, Input.MouseP)) {
            State.Hovered = UIID;
            if (Input.Mouse[0].WentDown) {
                State.Clicked = UIID;
                SetScene(UIID.Command->Scene);
            }
        }
    }
    State.InteractiveCount = 0;

    //
    //
    //

    scene *Scene = State.Scenes + State.CurrentScene;

    for (u32 i=0; i<Scene->CommandCount; ++i) {
        auto *Command = Scene->Commands + i;

        if (!Command->Retired) {
            if (Command->Active && State.Time >= Command->End)  {
                // todo: remove from the array
                if (Command->Type != Command_TextLayout) {
                    Command->Retired = true;
                    Command->Active = false;
                }
            } else if (State.Time >= Command->Start) {
                Command->Active = true;
            }
        }

        if (Command->Active) {
            r32 CharCount = Command->TextLength;
            v4 Color = Command->Color;

            if (Command->D.FadeIn && State.Time < Command->T.FadeInEnd) {
                // fade in
                r32 Value = Command->FadeInValue(State.Time);
                // Color.a *= Value;
                CharCount *= Value;
            } else if (Command->D.FadeOut && State.Time > Command->T.FadeOutStart) {
                // fade out
                r32 Value = 1.0f - Command->FadeOutValue(State.Time);
                Color.a *= Value;
            }

            if (CharCount > 0.0f) {
                if (Command->Type == Command_SimpleText) {
                    v2 P = State.Layout.Min;
                    DrawText(P, Color, Command->Font, Command->SizePx, Command->Text, CharCount);
                } else if (Command->Type == Command_TextLayout) {
                    rect Rect = TextLayout(Command->TextType, Command->Text, Command->TextLength, CharCount);
                    if (Command->TextType != Text_Normal) {
                        ui_id UIID = {};
                        UIID.Command = Command;
                        UIID.Rect = Rect;
                        State.Interactive[State.InteractiveCount++] = UIID;
                    }
                }
            }
        }
    }

    //
    //
    //

    if (Input.Mouse[0].WentUp) {
        State.Clicked = {};
    }

    State.Time += dT;
}