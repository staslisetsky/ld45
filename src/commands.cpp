void
CommandSimpleText(text_ TextType, r32 SpeedFactor, r32 ScreenTime, r32 FadeOut, char *Text)
{
    timed_command *Command = State.Commands + State.CommandCount++;

    Command->Start = State.CommandTime;

    Command->Text = Text;
    Command->Color = RGBA(255,255,255,255);
    Command->Font = Font_PTSans;
    Command->SizePx = 30.0f;
    Command->TextLength = strlen(Text);

    Command->D.FadeIn = Command->TextLength * State.SecondsPerGlyph * ( 1.0f / SpeedFactor);
    Command->D.ScreenTime = ScreenTime;
    Command->D.FadeOut = FadeOut;
    Command->T.FadeInStart = State.CommandTime;
    Command->T.FadeInEnd = Command->T.FadeInStart + Command->D.FadeIn;
    Command->T.FadeOutStart = Command->Start + Command->D.FadeIn + ScreenTime;
    Command->T.FadeOutEnd = Command->T.FadeOutStart + FadeOut;
    Command->End = Command->T.FadeOutEnd;
    Command->Duration = Command->End - Command->Start;

    State.CommandTime += Command->Duration;
}

timed_command *
CreateTextLayoutCommand(text_ TextType, char *Text, u32 TextLength)
{
    timed_command *Command = State.Commands + State.CommandCount++;
    Command->Type = Command_TextLayout;
    Command->Start = State.CommandTime;
    Command->TextType = TextType;
    Command->Text = Text;
    Command->TextLength = TextLength;
    Command->D.FadeIn = TextLength * State.SecondsPerGlyph;
    Command->T.FadeInStart = State.CommandTime;
    Command->T.FadeInEnd = Command->T.FadeInStart + Command->D.FadeIn;

    Command->Duration = Command->T.FadeInEnd -Command->T.FadeInStart;

    Command->End = Command->Start + Command->Duration;

    return Command;
}

void
CommandPause(r32 Duration)
{
    State.CommandTime += Duration;
}

void
CommandSpeed(r32 Factor)
{
    State.SecondsPerGlyph = 1.0 / 42.0f * (1.0f / Factor);
}

void
CommandTextLayout(text_ TextType, char *Text)
{
    ls_parser String = Text;

    char *PhraseStart = Text;

    while (String.RemainingBytes()) {
        token Token = String.GetToken();

        if (Token.Type == Token_OpenBracket) {
            b32 ParsingInlineCommands = true;

            if (String.At - PhraseStart > 1) {
                // note: " [0.1]" <--- this will work. We'll just create the command with one space character.
                auto *Command = CreateTextLayoutCommand(TextType, PhraseStart, String.At - 1 - PhraseStart);
                State.CommandTime += Command->Duration;
            }

            while (true) {
                token InlineCommand = String.GetToken();

                Assert(InlineCommand.Type == Token_Identifier);
                // note: commands should start with the letter
                //       eg for speed [s:0.1], for pauses: [p:1.0]

                String.RequireToken(Token_Colon);

                token Value = String.GetToken();
                Assert(Value.Type == Token_Real);

                if (InlineCommand.Text == "p") {
                    CommandPause(Value.Real);
                } else if (InlineCommand.Text == "s") {
                    CommandSpeed(Value.Real);
                }

                token Token = String.GetToken();

                if (Token.Type == Token_CloseBracket) {
                    String.RequireToken(Token_CloseBracket);
                    break;
                }

                Assert(Token.Type == Token_Comma);
                String.RequireToken(Token_Comma);
            }

            PhraseStart = String.At;
        }
    }

    u32 PhraseLength = strlen(PhraseStart);
    auto *Command = CreateTextLayoutCommand(TextType, PhraseStart, PhraseLength);

    // note: restore the default speed. We only alter this value in the current String
    State.SecondsPerGlyph = 1.0 / 42.0f;
    State.CommandTime += Command->Duration;
}


// void
// CommandBeginLayout(r32 PX, r32 PY, r32 DimX, r32 DimY)
// {
//     State.Layout.Min = v2{PX, PY};
//     State.Layout.Max = v2{PX + DimX, PY + DimY};
//     State.P = v2{PX, PY};
// }

