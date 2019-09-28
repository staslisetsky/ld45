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
    InputEvent_Count,
};

struct input_event {
    input_event_ Type;
    u32 Index;
};

#define MAX_INPUT_EVENTS 10

struct input {
    v2 MouseP;
    v2 MouseDragStartP;

    v2 dPMouse;
    s16 dWheel;

    button Mouse[2];
    button Keys[Key_Count];

    input_event Events[MAX_INPUT_EVENTS];
    u32 LastEvent;
    u32 FirstUnusedEvent;
};

struct read_file {
    u8 *Data;
    u32 Size;
};

bool PlatformReadFile(char *Filename, read_file *Result);