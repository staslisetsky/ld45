struct ui_id {
    u8 Id;
};

struct dude {
    v2 P;
};

struct docking_interface {
    v2 P;
    void *Module;
};

enum side_ {
    Side_Top,
    Side_Bottom,
    Side_Left,
    Side_Right,
    Side_Front,
    Side_Back,
    Side_Count,
};

struct module {
    docking_interface Interfaces[Side_Count];

    image Images[6];
    v2 Origins[6];

    image Image;
    v2 Origin;

    v2 P;
    v4 Tint;
};

enum module_ {
    Module_Main,
    Module_CylinderShort,
    Module_CylinderMedium,
    Module_CylinderLong,
    Module_Count,
};

struct station {
    image Door[16];
    module MainModule;
};

struct state {
    r32 Time;
    ui_id Hovered;
    ui_id Clicked;

    rect Container;
    rect Soil;
};

module ModuleLibrary[Module_Count];

state State = {};
station Station = {};