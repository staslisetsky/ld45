enum key_ {
    Key_Null,

    Key_A,
    Key_S,
    Key_D,
    Key_F,
    Key_H,
    Key_G,
    Key_Z,
    Key_X,
    Key_C,
    Key_V,
    Key_B,
    Key_Q,
    Key_W,
    Key_E,
    Key_R,
    Key_Y,
    Key_9,
    Key_7,
    Key_Minus,
    Key_8,
    Key_0,
    Key_CloseBracket,
    Key_O,
    Key_U,
    Key_OpenBracket,
    Key_I,
    Key_P,
    Key_Enter,
    Key_L,
    Key_J,
    Key_K,
    Key_Semicolon,
    Key_Backslash,
    Key_Comma,
    Key_ForwardSlash,
    Key_N,
    Key_M,
    Key_Dot,
    Key_Tab,
    Key_Space,
    Key_Backtick,
    Key_Backspace,
    Key_Esc,
    Key_Ctrl,
    Key_Shift,
    Key_CapsLock,
    Key_Alt,
    Key_F5,
    Key_F6,
    Key_F7,
    Key_F3,
    Key_F8,
    Key_F9,
    Key_F11,
    Key_F10,
    Key_F12,
    Key_Home,
    Key_PgUp,
    Key_Del,
    Key_F4,
    Key_End,
    Key_F2,
    Key_PgDown,
    Key_F1,
    Key_Left,
    Key_Right,
    Key_Down,
    Key_Up,
    Key_Count,
};

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