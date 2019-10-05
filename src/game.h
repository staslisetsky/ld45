
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
    Text_Play,
    Text_No,
    Text_Monster,
    Text_Panick,
    Text_Run,
    Text_Angry,
    Text_Ghostly,
    Text_Count,
};

enum scene_ {
    Scene_Intro,
    Scene_Start,
    Scene_Quit,
    Scene_Count,
};

enum command_ {
    Command_SimpleText,
    Command_TextLayout,
};

struct timed_command {
    b32 Active;
    b32 Retired;

    scene_ Scene;

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
    r32 SizePx;
    r32 SpeedFactor;

    r32 FadeInValue(r32 Time) { return (Time - this->T.FadeInStart) / (this->T.FadeInEnd - this->T.FadeInStart); }
    r32 FadeOutValue(r32 Time) { return (Time - this->T.FadeOutStart) / (this->T.FadeOutEnd - this->T.FadeOutStart); }
};

struct ui_id {
    timed_command *Command;
    rect Rect;
};

struct scene {
    timed_command Commands[50];
    u32 CommandCount;
};

struct state {
    r32 Time;
    r32 CommandTime;
    r32 SecondsPerGlyph;
    rect Layout;
    v2 P;

    scene_ CurrentScene;
    scene Scenes[Scene_Count];

    ui_id Hovered;
    ui_id Clicked;

    ui_id Interactive[10];
    u32 InteractiveCount;
};

state State = {};
