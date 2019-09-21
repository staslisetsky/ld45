void
Game(r32 dT)
{
    static u32 FrameCounter = 0;

    r32 X = (sin((r32)FrameCounter / 60.0f) + 1.0f) / 2.0f * 300.0f;
    r32 Y = (cos((r32)FrameCounter / 60.0f) + 1.0f) / 2.0f * 300.0f;
    DrawRect(&Render, v4{1.0f, 0.5f, 0.5f, 1.0f}, v2{X, Y}, v2{100.0f, 100.0f}, 1);
    ++FrameCounter;
}