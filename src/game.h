struct durations {
    r32 FadeIn;
    r32 ScreenTime;
    r32 FadeOut;
};

struct timings {
    r32 FadeInStart;
    r32 FadeInEnd;
    r32 FadeOutStart;
    r32 FadeOutEnd;
};

enum text_ {
    Text_Normal,
    Text_Monster,
    Text_Panick,
    Text_Run,
    Text_Angry,
    Text_Ghostly,
    Text_Count,
};

enum pos_ {
    Pos_Normal,
    Pos_Center,
};

enum command_ {
    Command_SimpleText,
    Command_TextLayout,
};

struct timed_command {
    b32 Active;
    b32 Retired;

    command_ Type;
    r32 Start;
    r32 End;
    r32 Duration;

    text_ TextType;

    durations D;
    timings T;

    // layout

    char *Text;
    u32 TextLength;

    v4 Color;
    v2 P;
    font_ Font;
    pos_ Pos;
    r32 SizePx;
    r32 SpeedFactor;

    r32 FadeInValue(r32 Time) { return (Time - this->T.FadeInStart) / (this->T.FadeInEnd - this->T.FadeInStart); }
    r32 FadeOutValue(r32 Time) { return (Time - this->T.FadeOutStart) / (this->T.FadeOutEnd - this->T.FadeOutStart); }
};

struct state {
    r32 Time;
    r32 CommandTime;
    r32 SecondsPerGlyph;

    rect Layout;
    v2 P;

    timed_command Commands[100];
    u32 CommandCount;
};

state State = {};
