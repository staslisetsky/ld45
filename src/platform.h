struct button {
    b32 Down;
    b32 WentDown;
    b32 DoubleClick;
    b32 WentUp;
    b32 WentDownOrRepeated;
};

struct input {
    v2 MouseP;
    v2 MouseDragStartP;

    v2 dPMouse;
    s16 dWheel;

    button Mouse[2];
    button Keys[Key_Count];
};

struct read_file {
    u8 *Data;
    u32 Size;
};