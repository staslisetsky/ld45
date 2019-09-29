struct image {
 u32 Texture;

 u32 Width;
 u32 Height;
 u32 N;

 u8 *Data;
};

void
DumpGlErrors(char *Section) {
 char Buf[100];

 while (true) {
  GLenum Error = glGetError();
  if (Error == GL_NO_ERROR) { break; }

  char *ErrorMessage = "";

  if (Error == GL_INVALID_ENUM) {
   ErrorMessage = "Invalid Enum";
} else if (Error == GL_INVALID_VALUE) {
   ErrorMessage = "Invalid Value";
} else if (Error == GL_INVALID_OPERATION) {
   ErrorMessage = "Invalid Operation";
} else if (Error == GL_INVALID_FRAMEBUFFER_OPERATION) {
   ErrorMessage = "Invalid Framebuffer Operation";
} else if (Error == GL_OUT_OF_MEMORY) {
   ErrorMessage = "Out of Memory";
}

char Buffer[100];
sprintf(Buffer, "[OpenGL] %s: %s\0", Section, ErrorMessage);
      // WasmConsoleLog(Buffer);
Assert(false);
}
}

GLuint
CreateShader(const char *String, GLuint Type)
{
 GLuint Shader = glCreateShader(Type);
 glShaderSource(Shader, 1, &String, 0);
 glCompileShader(Shader);

 GLint Status;
 glGetShaderiv(Shader, GL_COMPILE_STATUS, &Status);

 if (!Status) {
  GLint Length = 0;
  glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &Length);
  char *Log = (char *)malloc(Length);
  glGetShaderInfoLog(Shader, Length, 0, Log);
      // WasmConsoleLog("Shader compilation failed");
      // WasmConsoleLog(Log);
  Assert(false);
  free(Log);
}

return Shader;
}

GLuint
CreateProgram(const char *V, const char *F)
{
 GLuint Program = glCreateProgram();
 GLuint VShader = CreateShader(V, GL_VERTEX_SHADER);
 GLuint FShader = CreateShader(F, GL_FRAGMENT_SHADER);

 glAttachShader(Program, VShader);
 glAttachShader(Program, FShader);

 GLint GLProgramLinkStatus;
 glLinkProgram(Program);
 glGetProgramiv(Program, GL_LINK_STATUS, &GLProgramLinkStatus);

 if (GLProgramLinkStatus == GL_FALSE) {
    GLint Length;
    glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &Length);
    char *Log = (char *)malloc(Length);
    glGetProgramInfoLog(Program, Length, NULL, Log);
        // WasmConsoleLog("Shader linking failed");
        // WasmConsoleLog(Log);
    free(Log);
}


return Program;
}

GLuint
LoadShader(char *Name)
{
    ls_stringbuf Filename = "shaders/";
    Filename.AppendCString(Name);
    Filename.AppendCString(".v");

    read_file VertexFile;
    assert(PlatformReadFile(Filename.Data, &VertexFile));

    Filename = "shaders/";
    Filename.AppendCString(Name);
    Filename.AppendCString(".f");

    read_file FragmentFile;
    assert(PlatformReadFile(Filename.Data, &FragmentFile));

    return CreateProgram((char *)VertexFile.Data, (char *)FragmentFile.Data);
}

void
InitOpengl()
{
    glGenVertexArrays(1, &Render.VertexArrayPlain);
    glGenVertexArrays(1, &Render.VertexArrayTextured);
    glGenBuffers(1, &Render.VertexBufferPlain);
    glGenBuffers(1, &Render.VertexBufferTextured);

    // Plain vertex
    glBindVertexArray(Render.VertexArrayPlain);
    glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferPlain);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_xyzrgba) * 1000, 0, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgba), (GLvoid *)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgba), (GLvoid *)12);

       // Textured
    glBindVertexArray(Render.VertexArrayTextured);
    glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferTextured);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_xyzrgbauv) * 1000, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgbauv), (GLvoid *)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgbauv), (GLvoid *)12);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgbauv), (GLvoid *)28);

    glGenBuffers(1, &Render.ViewUniformBuffer);

    Render.Shaders[Shader_Plain].Name = "plain";
    Render.Shaders[Shader_Textured].Name = "textured";
    Render.Shaders[Shader_Glyph].Name = "glyph";

    for (u32 i=0; i<Shader_Count; ++i) {
        Render.Shaders[i].Id = LoadShader(Render.Shaders[i].Name.Data);
    }

    Render.PlainVertices = (vertex_xyzrgba *)malloc(sizeof(vertex_xyzrgba) * VERTEX_BUFFER_SIZE);
    Render.TexturedVertices = (vertex_xyzrgbauv *)malloc(sizeof(vertex_xyzrgbauv) * VERTEX_BUFFER_SIZE);
    Render.Commands = (render_command *)malloc(sizeof(render_command) * COMMAND_BUFFER_SIZE);

    DumpGlErrors("Bind buffers");
    glBindBuffer(GL_ARRAY_BUFFER, Render.ViewUniformBuffer);
    glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, Render.ViewUniformBuffer);

    GLuint Index = 0;
    Index = glGetUniformBlockIndex(Render.Shaders[Shader_Plain].Id, "view");
    glUniformBlockBinding(Render.Shaders[Shader_Plain].Id, Index, 0);

    Index = glGetUniformBlockIndex(Render.Shaders[Shader_Textured].Id, "view");
    glUniformBlockBinding(Render.Shaders[Shader_Textured].Id, Index, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    DumpGlErrors("Bind buffers");
}

GLuint
OpenglUploadTexture(image Image)
{
    GLuint Texture;
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);

    GLint ImageMode = GL_RGB;
    GLint InternalFormat = GL_RGB8;

    if (Image.N == 4) {
        ImageMode = GL_RGBA;
        InternalFormat = GL_RGBA8;
    } else if (Image.N == 1) {
        ImageMode = GL_RED;
        InternalFormat = GL_R8;
    }

    // note: Not GL_RED not supported in GL ES

    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat,
                 Image.Width, Image.Height,
                 0, ImageMode, GL_UNSIGNED_BYTE, Image.Data);
   // DumpGlErrors("Upload texture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_FILTER, GL_LINEAR);
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


  return Texture;
}

void
OpenglRender(render Render)
{
    m4x4 Projection = GetOrthoProjectionMatrix(0.0f, 1000.0f, Render.Screen.x, Render.Screen.y);

    glClearColor(0.73, 0.73, 0.73, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_UNIFORM_BUFFER, Render.ViewUniformBuffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m4x4), (u8 *)&Projection);

    if (Render.PlainVertexCount) {
        glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferPlain);
        glBufferSubData(GL_ARRAY_BUFFER, 0, Render.PlainVertexCount * sizeof(vertex_xyzrgba), (void *)Render.PlainVertices);
        DumpGlErrors("Upload verts");
    }

    if (Render.TexturedVertexCount) {
        glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferTextured);
        glBufferSubData(GL_ARRAY_BUFFER, 0, Render.TexturedVertexCount * sizeof(vertex_xyzrgbauv), (void *)Render.TexturedVertices);
    }

    for (u32 i=0; i<Render.CommandCount; ++i) {
        render_command Command = Render.Commands[i];
        glUseProgram(Render.Shaders[Command.Data.Shader].Id);

        if (Command.Data.Shader == Shader_Plain) {
            glBindVertexArray(Render.VertexArrayPlain);
            glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferPlain);
        } else if (Command.Data.Shader == Shader_Textured) {
            glBindVertexArray(Render.VertexArrayTextured);
            glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferTextured);
            glBindTexture(GL_TEXTURE_2D, Command.Data.Texture);
            // EM_ASM(console.log($0), Command.Data.Texture);
        } else if (Command.Data.Shader == Shader_Glyph) {
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