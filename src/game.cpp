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

    // auto *Command = TimedCommand(2.5f, 0.7f, 1.5f, 0.7f);
    // CommandSimpleText(v2{200.0f, 100.0f}, RGBA(255,255,255,255), Font_PTSans, 30.0f, "Hey.");

    // TimedCommand(Command->T.FadeOutStart, 0.7f, 1.5f, 0.7f);
    // CommandSimpleText(v2{200.0f, 150.0f}, RGBA(255,255,255,255), Font_PTSans, 30.0f, "Wanna play a little game?");

    // Stop();

    CommandBeginLayout(20.0f, 20.0f, Render.Screen.x - 40, Render.Screen.y - 40);
    CommandTextLayout(Text_Normal,
"So you walk into the room, right?[p:0.5]"
" And, like[p:0.09], there's this [s:0.4]giant Monster[s:1.0, p:0.3] just kinda standing right there.\n\n"
"[p:0.7,s:0.7] And he totally[p:0.7] fucking[p:0.5,s:0.8] sees you."
);

}

void
Game(r32 dT)
{
    State.Layout.Min = v2{50.0f, 40.0f};
    State.Layout.Max = v2{(r32)Render.Screen.x - 100.0f, (r32)Render.Screen.y - 80.0f};
    State.P = State.Layout.Min;

    for (u32 i=0; i<State.CommandCount; ++i) {
        auto *Command = State.Commands + i;

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
            if (Command->Type == Command_SimpleText) {
                if (State.Time < Command->T.FadeInEnd) {
                    // fade in
                    r32 Value = Command->FadeInValue(State.Time);
                    for (u32 j=0; j<Command->EntryCount; ++j) {
                        auto *Entry = Command->Entries + j;
                        v4 Color = Entry->Color;
                        Color.a *= Value;
                        DrawText(Entry->P, Color, Entry->Font, Entry->SizePx, Entry->Text);
                    }
                } else if (State.Time < Command->T.FadeOutStart) {
                    for (u32 j=0; j<Command->EntryCount; ++j) {
                        auto *Entry = Command->Entries + j;
                        DrawText(Entry->P, Entry->Color, Entry->Font, Entry->SizePx, Entry->Text);
                    }
                } else {
                    // fade out
                    r32 Value = 1.0f - Command->FadeOutValue(State.Time);
                    for (u32 j=0; j<Command->EntryCount; ++j) {
                        auto *Entry = Command->Entries + j;
                        v4 Color = Entry->Color;
                        Color.a *= Value;
                        DrawText(Entry->P, Color, Entry->Font, Entry->SizePx, Entry->Text);
                    }
                }
            } else if (Command->Type == Command_TextLayout) {
                r32 Value = (State.Time - Command->Start) / Command->Duration;

                u32 CharCount = Command->TextLength;
                if (Value < 1.0f) {
                    CharCount *= Value;
                }

                if (CharCount) {
                    TextLayout(Command->TextType, Command->Text, Command->TextLength, CharCount);
                }
            }
        }


    }

    State.Time += dT;
}