#define GL_TEXTURE_2D_MULTISAMPLE         0x9100

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_DEBUG_BIT_ARB         0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126

#define FRAMEBUFFER_SRGB 0x8DB9
#define SRGB8_ALPHA8 0x8C43

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_RGBA 0x1908
#define GL_R8 0x8229
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_REPEAT 0x2901
#define GL_CLAMP_TO_EDGE 0x812F

#define SYNC_GPU_COMMANDS_COMPLETE      0x9117

#define GL_MULTISAMPLE 0x809D
#define GL_READ_FRAMEBUFFER               0x8CA8
#define GL_DRAW_FRAMEBUFFER               0x8CA9
#define GL_RENDERBUFFER 0x8D41
#define GL_DEPTH24_STENCIL8 0x88F0

#define GL_FRAMEBUFFER  0x8D40
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT 0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED        0x8CDD
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 0x8CD9
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COLOR_ATTACHMENT1              0x8CE1
#define GL_COLOR_ATTACHMENT2              0x8CE2
#define GL_COLOR_ATTACHMENT3              0x8CE3
#define GL_COLOR_ATTACHMENT4              0x8CE4
#define GL_COLOR_ATTACHMENT5              0x8CE5
#define GL_COLOR_ATTACHMENT6              0x8CE6
#define GL_COLOR_ATTACHMENT7              0x8CE7
#define GL_COLOR_ATTACHMENT8              0x8CE8
#define GL_DEPTH_ATTACHMENT               0x8D00
#define GL_STENCIL_ATTACHMENT             0x8D20
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A

#define GL_SHADING_LANGUAGE_VERSION       0x8B8C

#define SRGB_EXT                                       0x8C40
#define SRGB8_EXT                                      0x8C41
#define SRGB_ALPHA_EXT                                 0x8C42
#define SRGB8_ALPHA8_EXT                               0x8C43
#define SLUMINANCE_ALPHA_EXT                           0x8C44
#define SLUMINANCE8_ALPHA8_EXT                         0x8C45
#define SLUMINANCE_EXT                                 0x8C46
#define SLUMINANCE8_EXT                                0x8C47
#define COMPRESSED_SRGB_EXT                            0x8C48
#define COMPRESSED_SRGB_ALPHA_EXT                      0x8C49
#define COMPRESSED_SLUMINANCE_EXT                      0x8C4A
#define COMPRESSED_SLUMINANCE_ALPHA_EXT                0x8C4B
#define TEXTURE_SRGB_DECODE_EXT        0x8A48
#define DECODE_EXT             0x8A49
#define SKIP_DECODE_EXT        0x8A4A

#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#define GL_CONTEXT_LOST 0x0507
#define GL_TABLE_TOO_LARGE 0x8031

#define GL_UNIFORM_BUFFER                 0x8A11
#define GL_UNIFORM_BUFFER_BINDING         0x8A28
#define GL_UNIFORM_BUFFER_START           0x8A29
#define GL_UNIFORM_BUFFER_SIZE            0x8A2A
#define GL_STENCIL_INDEX1                 0x8D46
#define GL_STENCIL_INDEX4                 0x8D47
#define GL_STENCIL_INDEX8                 0x8D48
#define GL_STENCIL_INDEX16                0x8D49

typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
typedef struct __GLsync *GLsync;
typedef int64_t GLint64;
typedef uint64_t GLuint64;

typedef BOOL WINAPI wgl_swap_interval_ext(int interval);
typedef HGLRC WINAPI wgl_create_context_attribs_arb(HDC hDC, HGLRC hShareContext, const int *attribList);
typedef const char *wgl_get_extensions_string_arb(HDC hdc);

typedef void gl_bind_buffer(GLenum target, GLuint buffer);
typedef void gl_attach_shader(GLuint Program, GLuint Shader);
typedef void gl_vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
typedef GLuint gl_create_shader(GLenum shaderType);
typedef void gl_compile_shader(GLuint shader);
typedef void gl_shader_source(GLuint shader, int count, const char **string, int *length);
typedef GLuint gl_create_program(void);
typedef void gl_link_program(GLuint program);
typedef void gl_gen_buffers(GLsizei n, GLuint * buffers);
typedef void gl_buffer_data(GLenum target, size_t size, const GLvoid * data, GLenum usage);
typedef void gl_get_program_iv(GLuint program, GLenum pname, GLint *params);
typedef void gl_get_shader_iv(GLuint shader, GLenum pname, GLint *params);
typedef void gl_use_program(GLuint Program);
typedef void gl_get_program_info_log(GLuint program, GLsizei maxLength, GLsizei *length, char *infoLog);
typedef void gl_tex_storage_2d(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
typedef void gl_shader_info_log(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
typedef void gl_gen_vertex_arrays(GLsizei n, GLuint *arrays);
typedef void gl_buffer_sub_data(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
typedef void gl_clear_buffer_fv(GLenum buffer, GLint drawbuffer, const GLfloat * value);
typedef GLint gl_get_uniform_location(GLuint program, const GLchar *name);
typedef void gl_uniform_1_ui(GLint location, GLuint v0);
typedef void gl_uniform_1_f(GLint location, GLfloat v0);
typedef void gl_uniform_1_f_v(GLint location, GLsizei count, const GLfloat *value);
typedef void gl_uniform_2_f(GLint location, GLfloat v0, GLfloat v1);
typedef void gl_uniform_2_f_v(GLint location, GLsizei count, const GLfloat *value);
typedef void gl_uniform_3_f_v(GLint location, GLsizei count, const GLfloat *value);
typedef void gl_uniform_4_f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void gl_uniform_4_f_v(GLint location, GLsizei count, const GLfloat *value);
typedef void gl_disable_vertex_attrib_array(GLuint Index);
typedef void gl_enable_vertex_attrib_array(GLuint Index);
typedef void gl_bind_sampler (GLuint unit, GLuint sampler);
typedef void gl_gen_samplers(GLsizei n, GLuint *samplers);
typedef void gl_sampler_parameteri(GLuint sampler, GLenum pname, GLint param);
typedef GLsync gl_fence_sync(GLenum condition, GLbitfield flags);
typedef GLenum gl_client_wait_sync(GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void gl_renderbuffer_storage_multisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void gl_uniform_matrix_4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
typedef void gl_bind_framebuffer(GLenum target, GLuint framebuffer);
typedef void gl_gen_framebuffers(GLsizei n, GLuint *ids);
typedef void gl_framebuffer_texture(GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void gl_draw_buffers( GLsizei n, const GLenum *bufs);
typedef void gl_framebuffer_texture_2d(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef GLenum gl_check_framebuffer_status(GLenum target);
typedef void gl_bind_vertex_array(GLuint array);
typedef void gl_bind_buffer_base(GLenum target, GLuint index, GLuint buffer);
typedef GLuint gl_get_uniform_block_index(GLuint program, const GLchar *uniformBlockName);
typedef void gl_uniform_block_binding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
typedef void gl_multidraw_elements_basevertex(GLenum mode, const GLsizei *count, GLenum type, const GLvoid **indices, GLsizei primcount, GLint *basevertex);
typedef void gl_multidraw_elements(GLenum mode, const GLsizei * count, GLenum type, const GLvoid **indices, GLsizei primcount);
typedef void gl_multidraw_arrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei primcount);
typedef void gl_teximage2d_multisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void gl_gen_framebuffers(GLsizei n, GLuint *framebuffers);
typedef void gl_bind_framebuffer(GLenum target, GLuint framebuffer);
typedef void gl_framebuffer_texture2d(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef void gl_blit_framebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
typedef void gl_draw_arrays_instaced(GLenum mode, GLint first, GLsizei count, GLsizei primcount);
typedef void gl_delete_program(GLuint program);

gl_use_program *glUseProgram;
gl_bind_buffer *glBindBuffer;
gl_attach_shader *glAttachShader;
gl_vertex_attrib_pointer *glVertexAttribPointer;
gl_create_shader *glCreateShader;
gl_compile_shader *glCompileShader;
gl_shader_source *glShaderSource;
gl_create_program *glCreateProgram;
gl_link_program *glLinkProgram;
gl_gen_buffers *glGenBuffers;
gl_buffer_data *glBufferData;
gl_get_program_iv *glGetProgramiv;
gl_get_shader_iv *glGetShaderiv;
gl_get_program_info_log *glGetProgramInfoLog;
gl_tex_storage_2d *glTexStorage2D;
gl_shader_info_log *glGetShaderInfoLog;
gl_gen_vertex_arrays *glGenVertexArrays;
gl_buffer_sub_data *glBufferSubData;
gl_clear_buffer_fv *glClearBufferfv;
gl_get_uniform_location *glGetUniformLocation;
gl_uniform_1_ui *glUniform1ui;
gl_uniform_1_f *glUniform1f;
gl_uniform_1_f_v *glUniform1fv;
gl_uniform_2_f *glUniform2f;
gl_uniform_2_f_v *glUniform2fv;
gl_uniform_3_f_v *glUniform3fv;
gl_uniform_4_f *glUniform4f;
gl_uniform_4_f_v *glUniform4fv;
gl_disable_vertex_attrib_array *glDisableVertexAttribArray;
gl_enable_vertex_attrib_array *glEnableVertexAttribArray;
gl_bind_sampler *glBindSampler;
gl_gen_samplers *glGenSamplers;
gl_sampler_parameteri *glSamplerParameteri;
gl_fence_sync *glFenceSync;
gl_client_wait_sync *glClientWaitSync;
gl_renderbuffer_storage_multisample *glRenderbufferStorageMultisample;
gl_uniform_matrix_4fv *glUniformMatrix4fv;
gl_bind_framebuffer *glBindFramebuffer;
gl_gen_framebuffers *glGenFramebuffers;
gl_framebuffer_texture *glFramebufferTexture;
gl_draw_buffers *glDrawBuffers;
gl_framebuffer_texture_2d *glFramebufferTexture2D;
gl_check_framebuffer_status *glCheckFramebufferStatus;
gl_bind_vertex_array *glBindVertexArray;
gl_draw_arrays_instaced *glDrawArraysInstanced;
wgl_get_extensions_string_arb *wglGetExtensionsStringARB;
gl_bind_buffer_base *glBindBufferBase;
gl_get_uniform_block_index *glGetUniformBlockIndex;
gl_uniform_block_binding *glUniformBlockBinding;
gl_multidraw_elements_basevertex *glMultiDrawElementsBaseVertex;
gl_multidraw_elements *glMultiDrawElements;
gl_multidraw_arrays *glMultiDrawArrays;
gl_teximage2d_multisample *glTexImage2DMultisample;
gl_blit_framebuffer *glBlitFramebuffer;
gl_delete_program *glDeleteProgram;

struct opengl_info {
    char *Vendor;
    char *Renderer;
    char *Version;
    char *ShadingLanguageVersion;
    char *Extensions;

    b32 SrgbTextureDecode;
    b32 FramebufferSrgb;
    b32 SrgbTextures;
};

void
InitOpenglContext(HWND Window)
{
    HDC DeviceContext = GetWindowDC(Window);
    HGLRC GLContext;
    PIXELFORMATDESCRIPTOR SuggestedPixelFormat;

    PIXELFORMATDESCRIPTOR DesiredPixelFormat = {
        sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd
        1,                                // version number
        PFD_DRAW_TO_WINDOW |              // support window
        PFD_SUPPORT_OPENGL |              // support OpenGL
        PFD_DOUBLEBUFFER,                 // double buffered
        PFD_TYPE_RGBA,                    // RGBA type
        24,                               // 24-bit color depth
        0, 0, 0, 0, 0, 0,                 // color bits ignored
        0,                                // no alpha buffer
        0,                                // shift bit ignored
        0,                                // no accumulation buffer
        0, 0, 0, 0,                       // accum bits ignored
        32,                               // 32-bit z-buffer
        8,                                // 8-bit stencil buffer
        0,                                // no auxiliary buffer
        PFD_MAIN_PLANE,                   // main layer
        0,                                // reserved
        0, 0, 0                           // layer masks ignored
    };

    int PixelFormatIndex = ChoosePixelFormat(DeviceContext, &DesiredPixelFormat);
    DescribePixelFormat(DeviceContext, PixelFormatIndex, sizeof(SuggestedPixelFormat), &SuggestedPixelFormat);
    SetPixelFormat(DeviceContext, PixelFormatIndex, &SuggestedPixelFormat);

    if (GLContext = wglCreateContext(DeviceContext)) {
        if (wglMakeCurrent(DeviceContext, GLContext)) {
            glUseProgram = (gl_use_program *)wglGetProcAddress("glUseProgram");
            glAttachShader = (gl_attach_shader *)wglGetProcAddress("glAttachShader");
            glBindBuffer = (gl_attach_shader *)wglGetProcAddress("glBindBuffer");
            glEnableVertexAttribArray = (gl_enable_vertex_attrib_array *)wglGetProcAddress("glEnableVertexAttribArray");
            glDisableVertexAttribArray = (gl_disable_vertex_attrib_array *)wglGetProcAddress("glDisableVertexAttribArray");
            glVertexAttribPointer = (gl_vertex_attrib_pointer *)wglGetProcAddress("glVertexAttribPointer");
            glCreateShader = (gl_create_shader *)wglGetProcAddress("glCreateShader");
            glCompileShader = (gl_compile_shader *)wglGetProcAddress("glCompileShader");
            glShaderSource = (gl_shader_source *)wglGetProcAddress("glShaderSource");
            glCreateProgram = (gl_create_program *)wglGetProcAddress("glCreateProgram");
            glLinkProgram = (gl_link_program *)wglGetProcAddress("glLinkProgram");
            glGenBuffers = (gl_gen_buffers *)wglGetProcAddress("glGenBuffers");
            glBufferData = (gl_buffer_data *)wglGetProcAddress("glBufferData");
            glGetShaderiv = (gl_get_shader_iv *)wglGetProcAddress("glGetShaderiv");
            glGetProgramiv = (gl_get_program_iv *)wglGetProcAddress("glGetProgramiv");
            glGetProgramInfoLog = (gl_get_program_info_log *)wglGetProcAddress("glGetProgramInfoLog");
            glTexStorage2D = (gl_tex_storage_2d *)wglGetProcAddress("glTexStorage2D");
            glGetShaderInfoLog = (gl_shader_info_log *)wglGetProcAddress("glGetShaderInfoLog");
            glGenVertexArrays = (gl_gen_vertex_arrays *)wglGetProcAddress("glGenVertexArrays");
            glBufferSubData = (gl_buffer_sub_data *)wglGetProcAddress("glBufferSubData");
            glClearBufferfv = (gl_clear_buffer_fv *)wglGetProcAddress("glClearBufferfv");
            glGetUniformLocation = (gl_get_uniform_location *)wglGetProcAddress("glGetUniformLocation");
            glUniform1ui = (gl_uniform_1_ui *)wglGetProcAddress("glUniform1ui");
            glUniform1f = (gl_uniform_1_f *)wglGetProcAddress("glUniform1f");
            glUniform1fv = (gl_uniform_1_f_v *)wglGetProcAddress("glUniform1fv");
            glUniform2f = (gl_uniform_2_f *)wglGetProcAddress("glUniform2f");
            glUniform2fv = (gl_uniform_2_f_v *)wglGetProcAddress("glUniform2fv");
            glUniform3fv = (gl_uniform_3_f_v *)wglGetProcAddress("glUniform3fv");
            glUniform4f = (gl_uniform_4_f *)wglGetProcAddress("glUniform4f");
            glUniform4fv = (gl_uniform_4_f_v *)wglGetProcAddress("glUniform4fv");
            glUniformMatrix4fv = (gl_uniform_matrix_4fv *)wglGetProcAddress("glUniformMatrix4fv");
            glBindSampler = (gl_bind_sampler *)wglGetProcAddress("glBindSampler");
            glGenSamplers = (gl_gen_samplers *)wglGetProcAddress("glGenSamplers");
            glSamplerParameteri = (gl_sampler_parameteri *)wglGetProcAddress("glSamplerParameteri");
            glFenceSync = (gl_fence_sync *)wglGetProcAddress("glFenceSync");
            glClientWaitSync = (gl_client_wait_sync *)wglGetProcAddress("glClientWaitSync");
            glRenderbufferStorageMultisample = (gl_renderbuffer_storage_multisample *)wglGetProcAddress("glRenderbufferStorageMultisample");
            glGenFramebuffers = (gl_gen_framebuffers *)wglGetProcAddress("glGenFramebuffers");
            glBindFramebuffer = (gl_bind_framebuffer *)wglGetProcAddress("glBindFramebuffer");
            glDeleteProgram = (gl_delete_program *)wglGetProcAddress("glDeleteProgram");

            glFramebufferTexture = (gl_framebuffer_texture *)wglGetProcAddress("glFramebufferTexture");
            glDrawBuffers = (gl_draw_buffers *)wglGetProcAddress("glDrawBuffers");
            glFramebufferTexture2D = (gl_framebuffer_texture_2d *)wglGetProcAddress("glFramebufferTexture2D");
            glCheckFramebufferStatus = (gl_check_framebuffer_status *)wglGetProcAddress("glCheckFramebufferStatus");

            glBindVertexArray = (gl_bind_vertex_array *)wglGetProcAddress("glBindVertexArray");
            wglGetExtensionsStringARB = (wgl_get_extensions_string_arb *)wglGetProcAddress("wglGetExtensionsStringARB");
            // glDebugMessageCallback = (gl_debug_message_callback *)wglGetProcAddress("glDebugMessageCallback");

            glBindBufferBase = (gl_bind_buffer_base *)wglGetProcAddress("glBindBufferBase");
            glGetUniformBlockIndex = (gl_get_uniform_block_index *)wglGetProcAddress("glGetUniformBlockIndex");
            glUniformBlockBinding = (gl_uniform_block_binding *)wglGetProcAddress("glUniformBlockBinding");
            glMultiDrawElementsBaseVertex = (gl_multidraw_elements_basevertex *)wglGetProcAddress("glMultiDrawElementsBaseVertex");
            glMultiDrawElements = (gl_multidraw_elements *)wglGetProcAddress("glMultiDrawElements");
            glMultiDrawArrays = (gl_multidraw_arrays *)wglGetProcAddress("glMultiDrawArrays");
            glTexImage2DMultisample = (gl_teximage2d_multisample *)wglGetProcAddress("glTexImage2DMultisample");

            glGenFramebuffers = (gl_gen_framebuffers *)wglGetProcAddress("glGenFramebuffers");
            glBindFramebuffer = (gl_bind_framebuffer *)wglGetProcAddress("glBindFramebuffer");
            glFramebufferTexture2D = (gl_framebuffer_texture2d *)wglGetProcAddress("glFramebufferTexture2D");

            glBlitFramebuffer = (gl_blit_framebuffer *)wglGetProcAddress("glBlitFramebuffer");
            glDrawArraysInstanced = (gl_draw_arrays_instaced *)wglGetProcAddress("glDrawArraysInstanced");

        }
    }

    b32 ModernContext = false;

    wgl_swap_interval_ext *wglSwapInterval = (wgl_swap_interval_ext *)wglGetProcAddress("wglSwapIntervalEXT");
    wgl_create_context_attribs_arb *wglCreateContextAttribsARB = (wgl_create_context_attribs_arb *)wglGetProcAddress("wglCreateContextAttribsARB");

    if (wglCreateContextAttribsARB) {
        int Attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 3,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            WGL_CONTEXT_PROFILE_MASK_ARB, 1,
            0 // should be terminated by 0
        };

        HGLRC SharedContext =  0;
        HGLRC ModernGLRC = wglCreateContextAttribsARB(DeviceContext, SharedContext, Attribs);

        if (ModernGLRC) {
            if (wglMakeCurrent(DeviceContext, ModernGLRC)) {
                ModernContext = true;
                wglDeleteContext(GLContext);
                GLContext = ModernGLRC;

                // PlatformWriteLog("[OpenGL] Using modern context\n");
            }
        } else {
            // PlatformWriteLog("[OpenGL] Using Legacy Context\n");
        }
    }

    if (wglSwapInterval) {
        // PlatformWriteLog("[OpenGL] Using VSync\n");
        wglSwapInterval(1);
    }

    opengl_info Info = {};

    Info.Vendor = (char *)glGetString(GL_VENDOR);
    Info.Renderer = (char *)glGetString(GL_RENDERER);
    Info.Version = (char *)glGetString(GL_VERSION);

    if (ModernContext) {
        Info.ShadingLanguageVersion = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
        Info.Extensions = (char *)wglGetExtensionsStringARB(DeviceContext);
    } else {
        Info.Extensions = (char *)glGetString(GL_EXTENSIONS);
    }

    ls_parser Ext = Info.Extensions;

    while (Ext.RemainingBytes()) {
        token Token = Ext.GetToken();

        if (Token.Text == "EXT_texture_sRGB_decode") {
            Info.SrgbTextureDecode = true;
        } else if (Token.Text == "WGL_EXT_framebuffer_sRGB" ||
                   Token.Text == "GL_ARB_framebuffer_sRGB"  ||
                   Token.Text == "WGL_ARB_framebuffer_sRGB")
        {
            Info.FramebufferSrgb = true;
        } else if (Token.Text == "GL_EXT_texture_sRGB" ||
                   Token.Text == "GL_ARB_texture_sRGB")
        {
            Info.SrgbTextures = true;
        }
    }

    if (Info.FramebufferSrgb) {
        glEnable(FRAMEBUFFER_SRGB);
    }

    ReleaseDC(Window, DeviceContext);
}


