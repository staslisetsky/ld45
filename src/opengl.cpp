/*

Opengl NDC coordinates

-1,1     1,1
  ┌──────┐
  │      │
  │      │
  └──────┘
-1,-1    1,-1

*/

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
        char *Log = (char *)malloc(Length + 1);
        glGetShaderInfoLog(Shader, Length, 0, Log);
        Log[Length] = 0;

        os::PrintLog("opengl", Log);
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
        os::PrintLog("opengl", Log);
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
    assert(os::ReadFile(Filename.Data, &VertexFile));

    Filename = "shaders/";
    Filename.AppendCString(Name);
    Filename.AppendCString(".f");

    read_file FragmentFile;
    assert(os::ReadFile(Filename.Data, &FragmentFile));

    return CreateProgram((char *)VertexFile.Data, (char *)FragmentFile.Data);
}

void
InitOpengl()
{
    glGenVertexArrays(1, &Renderer.VertexArrayPlain);
    glGenVertexArrays(1, &Renderer.VertexArrayTextured);
    glGenBuffers(1, &Renderer.VertexBufferPlain);
    glGenBuffers(1, &Renderer.VertexBufferTextured);

    // Plain vertex
    glBindVertexArray(Renderer.VertexArrayPlain);
    glBindBuffer(GL_ARRAY_BUFFER, Renderer.VertexBufferPlain);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_xyzrgba) * 1000, 0, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgba), (GLvoid *)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgba), (GLvoid *)12);

       // Textured
    glBindVertexArray(Renderer.VertexArrayTextured);
    glBindBuffer(GL_ARRAY_BUFFER, Renderer.VertexBufferTextured);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_xyzrgbauv) * 1000, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgbauv), (GLvoid *)0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgbauv), (GLvoid *)12);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgbauv), (GLvoid *)28);

    glGenBuffers(1, &Renderer.ViewUniformBuffer);

    Renderer.Shaders[Shader_Plain].Name = "plain";
    Renderer.Shaders[Shader_Textured].Name = "textured";
    Renderer.Shaders[Shader_Glyph].Name = "glyph";

    for (u32 i=0; i<Shader_Count; ++i) {
        Renderer.Shaders[i].Id = LoadShader(Renderer.Shaders[i].Name.Data);
    }

    Renderer.PlainVertices = (vertex_xyzrgba *)malloc(sizeof(vertex_xyzrgba) * VERTEX_BUFFER_SIZE);
    Renderer.TexturedVertices = (vertex_xyzrgbauv *)malloc(sizeof(vertex_xyzrgbauv) * VERTEX_BUFFER_SIZE);
    // Renderer.Commands = (render_command *)malloc(sizeof(render_command) * COMMAND_BUFFER_SIZE);

    DumpGlErrors("Bind buffers");
    glBindBuffer(GL_ARRAY_BUFFER, Renderer.ViewUniformBuffer);
    glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, Renderer.ViewUniformBuffer);

    GLuint Index = 0;
    Index = glGetUniformBlockIndex(Renderer.Shaders[Shader_Plain].Id, "view");
    glUniformBlockBinding(Renderer.Shaders[Shader_Plain].Id, Index, 0);

    Index = glGetUniformBlockIndex(Renderer.Shaders[Shader_Textured].Id, "view");
    glUniformBlockBinding(Renderer.Shaders[Shader_Textured].Id, Index, 0);

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
        InternalFormat = GL_RED;
    }

    // note: Not GL_RED not supported in GL ES

    glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat,
                 Image.Width, Image.Height,
                 0, ImageMode, GL_UNSIGNED_BYTE, Image.Data);
    DumpGlErrors("Upload texture");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_FILTER, GL_LINEAR);
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
   // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


  return Texture;
}
