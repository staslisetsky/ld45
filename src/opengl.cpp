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
// Assert(false);
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
  // Assert(false);
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

    const char *PlainV = R"str(#version 300 es
    in vec4 In_P;
    in vec4 In_VertexColor;
    out vec4 VertexColor;

    layout(std140) uniform view
    {
        mat4 Projection;
    } View;

    void main()
    {
        gl_Position = View.Projection * In_P;
        VertexColor = In_VertexColor;
    }

    )str";

    const char *PlainF = R"str(#version 300 es
    precision mediump float;
    in vec4 VertexColor;
    out vec4 FragmentColor;
    void main(void)
    {
        FragmentColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    )str";

    const char *TexturedV = R"str(#version 300 es

    in vec4 In_P;
    in vec4 In_VertexColor;
    in vec2 In_TexelUV;

    out vec4 VertexColor;
    out vec2 TexelUV;

    layout(std140) uniform view
    {
        mat4 Projection;
    } View;

    void main()
    {
        gl_Position = View.Projection * In_P;
        VertexColor = In_VertexColor;
        TexelUV = In_TexelUV;
    }
    )str";

    const char *TexturedF = R"str(#version 300 es
    precision mediump float;
    uniform sampler2D TextureSample;
    in vec4 VertexColor;
    in vec2 TexelUV;
    out vec4 FragmentColor;
    void main(void)
    {
        FragmentColor = texture(TextureSample, TexelUV);
        // FragmentColor = vec4(0.3, 0.7, 0.22, 1.0);
    }
    )str";

    const char *SDFV = R"str(#version 300 es
    in vec4 In_P;
    in vec4 In_VertexColor;
    in vec2 In_TexelUV;

    out vec4 VertexColor;
    out vec2 TexelUV;

    layout(std140) uniform view
    {
        mat4 Projection;
    } View;

    void main()
    {
        gl_Position = View.Projection * In_P;
        VertexColor = In_VertexColor;
        TexelUV = In_TexelUV;
    }
    )str";

    const char *SDFF = R"str(#version 300 es
    precision mediump float;
    uniform sampler2D TextureSample;
    uniform vec2 QuadDim;

    in vec4 VertexColor;
    in vec2 TexelUV;
    out vec4 FragmentColor;

    float median(float r, float g, float b) {
        return max(min(r, g), min(max(r, g), b));
    }

    void main() {
        vec2 msdfUnit = QuadDim / vec2(textureSize(TextureSample, 0));
        vec3 SampleColor = texture(TextureSample, TexelUV).rgb;

        float sigDist = median(SampleColor.r, SampleColor.g, SampleColor.b) - 0.5;
        sigDist *= dot(msdfUnit, 0.5/fwidth(TexelUV));
        float opacity = clamp(sigDist + 0.5, 0.0, 1.0);
        //color = mix(bgColor, fgColor, opacity);
        FragmentColor = vec4(1.0, 0.0, 0.0, 1.0) * opacity;

        //FragmentColor = texture(TextureSample, TexelUV);
    }
    )str";

    Render.PlainShader = CreateProgram(PlainV, PlainF);
    Render.TexturedShader = CreateProgram(TexturedV, TexturedF);
    Render.SDFShader = CreateProgram(SDFV, SDFF);

    Render.PlainVertices = (vertex_xyzrgba *)malloc(sizeof(vertex_xyzrgba) * 100);
    Render.TexturedVertices = (vertex_xyzrgbauv *)malloc(sizeof(vertex_xyzrgbauv) * 100);

    Render.Commands = (render_command *)malloc(sizeof(render_command) * 100);

    DumpGlErrors("Bind buffers");
    glBindBuffer(GL_ARRAY_BUFFER, Render.ViewUniformBuffer);
    glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, Render.ViewUniformBuffer);

    GLuint Index = 0;
    Index = glGetUniformBlockIndex(Render.PlainShader, "view");
    glUniformBlockBinding(Render.PlainShader, Index, 0);

    Index = glGetUniformBlockIndex(Render.TexturedShader, "view");
    glUniformBlockBinding(Render.TexturedShader, Index, 0);

    Index = glGetUniformBlockIndex(Render.SDFShader, "view");
    glUniformBlockBinding(Render.SDFShader, Index, 0);

    Render.QuadDimUniform = glGetUniformLocation(Render.SDFShader, "QuadDim");

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

glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat,
             Image.Width, Image.Height,
             0, ImageMode, GL_UNSIGNED_BYTE, Image.Data);
   // DumpGlErrors("Upload texture");

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
        glUseProgram(Command.Data.Shader);

        if (Command.Data.Shader == Render.PlainShader) {
            glBindVertexArray(Render.VertexArrayPlain);
            glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferPlain);
            DumpGlErrors("Bind buffers");
        } else if (Command.Data.Shader == Render.TexturedShader) {
            glBindVertexArray(Render.VertexArrayTextured);
            glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferTextured);
            glBindTexture(GL_TEXTURE_2D, Command.Data.Texture);
            // EM_ASM(console.log($0), Command.Data.Texture);
        } else if (Command.Data.Shader == Render.SDFShader) {
            glUniform2f(Render.QuadDimUniform, (r32)Command.Data.QuadDim.x, (r32)Command.Data.QuadDim.y);
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