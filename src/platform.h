struct button {
    b32 Down;
    b32 WentDown;
    b32 DoubleClick;
    b32 WentUp;
    b32 WentDownOrRepeated;
};

enum input_event_ {
    InputEvent_Null,

    InputEvent_MouseDown,
    InputEvent_MouseUp,
    InputEvent_KeyUp,
    InputEvent_KeyDown,
    InputEvent_KeyDownRepeated,
    InputEvent_Count,
};

struct input_event {
    input_event_ Type;
    u32 Index;
};

#define MAX_INPUT_EVENTS 10

struct input {
    v2 MouseP;
    v2 MouseDP;
    v2 WMouseP;
    v2 WMouseDP;
    s16 MouseWheel;

    button Mouse[2];
    button Keys[Key_Count];

    input_event Events[MAX_INPUT_EVENTS];
    volatile u32 LastEvent;
    volatile u32 LastUsedEvent;
};

struct read_file {
    u8 *Data;
    u32 Size;
};

struct os {
    static bool ReadFile(char *Filename, read_file *Result);
    static void PrintLog(char *Section, char *Log);
};

