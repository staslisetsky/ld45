struct renderer {
    m4x4 ProjectionMatrix;

    vertex_xyzrgba *PlainVertices;
    u32 PlainVertexCount;
    // vertex_xyzrgbauv *TexturedVertices;
    // u32 TexturedVertexCount;
};