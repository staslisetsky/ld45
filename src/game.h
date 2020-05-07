struct game {
    r32 Time;
    // ui_id Hovered;
    // ui_id Clicked;

    rect Container;
    rect Soil;

    v2 CameraP;
    r32 CameraScale = 1.0f;
};

game Game = {};
