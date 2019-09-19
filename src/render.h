enum draw_mode_ {
    DrawMode_Quad,
    DrawMode_Triangle,
    DrawMode_Strip,
};

struct command_data {
    GLuint Shader;
    GLuint Texture;
};

struct render_command {
    draw_mode_ DrawMode;
    u32 Offset;
    u32 PrimitiveCount;
    command_data Data;
};

struct vertex_xyzrgba {
    v3 P;
    v4 Color;
};

struct vertex_xyzrgbauv {
    v3 P;
    v4 Color;
    v2 UV;
};

struct render {
    GLuint VertexArrayPlain;
    GLuint VertexArrayTextured;
    GLuint VertexBufferPlain;
    GLuint VertexBufferTextured;
    GLuint ViewUniformBuffer;

    GLuint PlainShader;
    GLuint TexturedShader;
    GLuint GlyphShader;

    GLuint TestTexture;

    m4x4 ProjectionMatrix;

    vertex_xyzrgba *PlainVertices;
    u32 PlainVertexCount;

    vertex_xyzrgbauv *TexturedVertices;
    u32 TexturedVertexCount;

    render_command *Commands;
    u32 CommandCount;
};


m4x4
GetOrthoProjectionMatrix(r32 Near, r32 Far, r32 ScreenWidth, r32 ScreenHeight)
{
    m4x4 Result = {};

    r32 a = SafeDivide1(2.0f, ScreenWidth);
    r32 b = SafeDivide1(2.0f, ScreenHeight);
    r32 c = SafeDivide1(1.0f, Far - Near);

    Result = {
        {a,  0,  0,  0,
         0, -b,  0,  0,
         0,  0, -c,  0,
        -1,  1,  0,  1
        }
    };

    return Result;
}

void
AddRenderCommand(render *Render, draw_mode_ Mode, u32 Offset, u32 PrimitiveCount, command_data Data)
{
    render_command *Command = 0;
    if (!Render->CommandCount) {
        Command = Render->Commands + 0;
        Command->PrimitiveCount = PrimitiveCount;
        Command->Offset = Offset;
        Command->DrawMode = Mode;
        Command->Data = Data;
        ++Render->CommandCount;
    } else {
        render_command *LastCommand = Render->Commands + Render->CommandCount - 1;

        if (LastCommand->Data.Shader != Data.Shader ||
            LastCommand->Data.Texture != Data.Texture)
        {
            Command = Render->Commands + Render->CommandCount;
            Command->PrimitiveCount = PrimitiveCount;
            Command->Offset = Offset;
            Command->DrawMode = Mode;
            Command->Data = Data;
            ++Render->CommandCount;
        } else {
            LastCommand->PrimitiveCount += PrimitiveCount;
        }
    }
}

void
DrawRect(render *Render, v4 Color, v2 P, v2 Dim, r32 Z)
{
    vertex_xyzrgba *Vertices = Render->PlainVertices + Render->PlainVertexCount;

    Vertices[0].P = v3{P.x, P.y, Z};
    Vertices[1].P = v3{P.x + Dim.x, P.y, Z};
    Vertices[2].P = v3{P.x, P.y + Dim.y, Z};
    Vertices[3].P = v3{P.x + Dim.x, P.y + Dim.y, Z};

    Vertices[0].Color = Color;
    Vertices[1].Color = Color;
    Vertices[2].Color = Color;
    Vertices[3].Color = Color;

    command_data Data = {};
    Data.Shader = Render->PlainShader;
    AddRenderCommand(Render, DrawMode_Strip, Render->PlainVertexCount, 4, Data);
    Render->PlainVertexCount += 4;
}

void
DrawTexturedRect(render *Render, v2 P, v2 Dim, v4 Color, u32 Texture, u32 Z=0)
{
    vertex_xyzrgbauv *Vertices = Render->TexturedVertices + Render->TexturedVertexCount;

    Vertices[0].P = v3{P.x, P.y, (r32)Z};
    Vertices[1].P = v3{P.x + Dim.x, P.y, (r32)Z};
    Vertices[2].P = v3{P.x, P.y + Dim.y, (r32)Z};
    Vertices[3].P = v3{P.x + Dim.x, P.y + Dim.y, (r32)Z};

    Vertices[0].Color = Color;
    Vertices[1].Color = Color;
    Vertices[2].Color = Color;
    Vertices[3].Color = Color;

    Vertices[0].UV = v2{0.0f, 0.0f};
    Vertices[1].UV = v2{1.0f, 0.0f};
    Vertices[2].UV = v2{0.0f, 1.0f};
    Vertices[3].UV = v2{1.0f, 1.0f};

    command_data Data = {};
    Data.Shader = Render->TexturedShader;
    Data.Texture = Render->TestTexture;
    AddRenderCommand(Render, DrawMode_Strip, Render->TexturedVertexCount, 4, Data);
    Render->TexturedVertexCount += 4;
}

void
RenderCommands(render Render)
{
    m4x4 Projection = GetOrthoProjectionMatrix(0.0f, 1000.0f, 1000.0f, 500.0f);

    glClearColor(0.73, 0.73, 0.73, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_UNIFORM_BUFFER, Render.ViewUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m4x4), (u8 *)&Projection);

    if (Render.PlainVertexCount) {
        glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferPlain);
        glBufferSubData(GL_ARRAY_BUFFER, 0, Render.PlainVertexCount * sizeof(vertex_xyzrgba), (void *)Render.PlainVertices);
    }

    if (Render.TexturedVertexCount) {
        glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferTextured);
        glBufferSubData(GL_ARRAY_BUFFER, 0, Render.TexturedVertexCount * sizeof(vertex_xyzrgbauv), (void *)Render.TexturedVertices);
    }

    for (u32 i=0; i<Render.CommandCount; ++i) {
        render_command Command = Render.Commands[i];
        glUseProgram(Command.Data.Shader);

        if (Command.Data.Shader == Render.PlainShader) {
            glBindVertexArray(Render.VertexArrayPlain);
            glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferPlain);
        } else if (Command.Data.Shader == Render.TexturedShader) {
            glBindVertexArray(Render.VertexArrayTextured);
            glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferTextured);
            glBindTexture(GL_TEXTURE_2D, Command.Data.Texture);
            // EM_ASM(console.log($0), Command.Data.Texture);
        }

        if (Command.DrawMode == DrawMode_Quad) {
            glDrawArrays(GL_TRIANGLES, Command.Offset, 6 * Command.PrimitiveCount);
        } else if (Command.DrawMode == DrawMode_Triangle) {
            glDrawArrays(GL_TRIANGLES, Command.Offset, 3 * Command.PrimitiveCount);
        } else if (Command.DrawMode == DrawMode_Strip) {
            glDrawArrays(GL_TRIANGLE_STRIP, Command.Offset, Command.PrimitiveCount);
        }
    }
}