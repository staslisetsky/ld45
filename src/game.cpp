void
Game(r32 dT)
{
    static u32 FrameCounter = 0;

    r32 X = (sin((r32)FrameCounter / 60.0f) + 1.0f) / 2.0f * 300.0f + 250.0f;
    r32 Y = (cos((r32)FrameCounter / 60.0f) + 1.0f) / 2.0f * 300.0f + 50.0f;
    r32 Z = (sin((r32)FrameCounter / 60.0f) + 1.0f) / 2.0f * 200.0f + 100.0f;
    DrawGlyph(&Render, v2{X, Y}, v2{Z, Z}, v4{1.0f, 0.5f, 0.5f, 1.0f}, Render.TestTexture, 1);
    // DrawGlyph(&Render, v2{X, Y}, v2{100.0f, 100.0f}, v4{1.0f, 0.5f, 0.5f, 1.0f}, Render.TestTexture, 1);
    ++FrameCounter;
}