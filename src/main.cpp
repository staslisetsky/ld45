#include <emscripten/emscripten.h>
#include <emscripten/key_codes.h>
#include <emscripten/html5.h>

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include <GLES3/gl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

typedef char utf8;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s32 b32;
typedef float r32;
typedef double r64;

void
WasmConsoleLog(const char *String)
{
   EM_ASM(console.log(UTF8ToString($0)), String);
}

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
      WasmConsoleLog(Buffer);
   }
}

#include "ls_math.h"
#include "render.h"
#include "platform.h"

struct wasm_state {
   EMSCRIPTEN_WEBGL_CONTEXT_HANDLE WebGLContext;
   u32 Frame;
};

wasm_state State = {};
render Render = {};
input Input = {};

void
WasmPrintError(EMSCRIPTEN_RESULT Error)
{
   switch(Error) {
      case EMSCRIPTEN_RESULT_SUCCESS: {
        WasmConsoleLog("The operation succeeded.");
      } break;
      case EMSCRIPTEN_RESULT_DEFERRED: {
         WasmConsoleLog("The requested operation cannot be completed now for web security reasons, and has been deferred for completion in the next event handler.");
      } break;
      case EMSCRIPTEN_RESULT_NOT_SUPPORTED: {
         WasmConsoleLog("The given operation is not supported by this browser or the target element. This value will be returned at the time the callback is registered if the operation is not supported.");
      } break;
      case EMSCRIPTEN_RESULT_FAILED_NOT_DEFERRED: {
         WasmConsoleLog("The requested operation could not be completed now for web security reasons. It failed because the user requested the operation not be deferred.");
      } break;
      case EMSCRIPTEN_RESULT_INVALID_TARGET: {
         WasmConsoleLog("The operation failed because the specified target element is invalid.");
      } break;
      case EMSCRIPTEN_RESULT_UNKNOWN_TARGET: {
         WasmConsoleLog("The operation failed because the specified target element was not found.");
      } break;
      case EMSCRIPTEN_RESULT_INVALID_PARAM: {
         WasmConsoleLog("The operation failed because an invalid parameter was passed to the function.");
      } break;
      case EMSCRIPTEN_RESULT_FAILED: {
         WasmConsoleLog("Generic failure result message, returned if no specific result is available.");
      } break;
      case EMSCRIPTEN_RESULT_NO_DATA: {
         WasmConsoleLog("The operation failed because no data is currently available.");
      } break;
      default: {
         WasmConsoleLog("Unknown error");
      }
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
      WasmConsoleLog("Shader compilation failed");
      WasmConsoleLog(Log);
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
        WasmConsoleLog("Shader linking failed");
        WasmConsoleLog(Log);
        free(Log);
   }


   return Program;
}

void
WasmInitOpengl()
{
   glGenBuffers(1, &Render.VertexArrayPlain);
   glGenBuffers(1, &Render.VertexArrayTextured);
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

   Render.PlainShader = CreateProgram(PlainV, PlainF);
   Render.TexturedShader = CreateProgram(TexturedV, TexturedF);
   Render.PlainVertices = (vertex_xyzrgba *)malloc(sizeof(vertex_xyzrgba) * 100);
   Render.TexturedVertices = (vertex_xyzrgbauv *)malloc(sizeof(vertex_xyzrgbauv) * 100);
   Render.Commands = (render_command *)malloc(sizeof(render_command) * 100);

   glBindBuffer(GL_ARRAY_BUFFER, Render.ViewUniformBuffer);
   glBufferData(GL_ARRAY_BUFFER, 64, NULL, GL_DYNAMIC_DRAW);
   glBindBufferBase(GL_UNIFORM_BUFFER, 0, Render.ViewUniformBuffer);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void
WasmMainLoop()
{
   for (u32 i=0; i<Key_Count; ++i) {
      Input.Keys[i].WentDown = false;
      Input.Keys[i].WentUp = false;
      Input.Keys[i].WentDownOrRepeated = false;
   }
   Input.Mouse[0].WentDown = false;
   Input.Mouse[0].WentUp = false;
   Input.Mouse[1].WentDown = false;
   Input.Mouse[1].WentUp = false;

   // r32 X = (sin((r32)State.Frame / 100.0f) + 1.0f / 2.0f) * 100.0f + 500.0f;
   // r32 Y = (cos((r32)State.Frame / 100.0f) + 1.0f / 2.0f) * 100.0f + 100.0f;

   static r32 X = 0;
   static r32 Y = 0;

   if (Input.Keys[Key_Left].Down) {
      X -= 1.0f;
   }
   if (Input.Keys[Key_Right].Down) {
      X += 1.0f;
   }
   if (Input.Keys[Key_Up].Down) {
      Y -= 1.0f;
   }
   if (Input.Keys[Key_Down].Down) {
      Y += 1.0f;
   }

   DrawTexturedRect(&Render, Input.MouseP, v2{200.0f, 200.0f}, v4{1.0f,0.5f,0.5f,1.0f}, Render.TestTexture, 1);

   RenderCommands(Render);

   // swap:
   // emscripten_webgl_commit_frame();
   Render.PlainVertexCount = 0;
   Render.TexturedVertexCount = 0;
   Render.CommandCount = 0;

   ++State.Frame;
}

EM_BOOL
WasmKeyMouseEventCallback(s32 EventType, const EmscriptenMouseEvent *Event, void *UserData)
{
   u32 Button = 0;
   if (Event->button == 2) {
      Button = 1;
   }

   if (EventType == EMSCRIPTEN_EVENT_MOUSEDOWN) {
      Input.Mouse[Button].WentDown = true;
      Input.Mouse[Button].Down = true;
      // EM_ASM(console.log('Mouse button '  + $0 + 'down'), Button);
   } else if (EventType == EMSCRIPTEN_EVENT_MOUSEUP) {
      Input.Mouse[Button].WentUp = true;
      Input.Mouse[Button].Down = false;
      // EM_ASM(console.log('Mouse button '  + $0 + 'up'), Button);
   } else if (EventType == EMSCRIPTEN_EVENT_DBLCLICK) {

   } else if (EventType == EMSCRIPTEN_EVENT_MOUSEMOVE) {
      Input.MouseP.x = (r32)Event->targetX;
      Input.MouseP.y = (r32)Event->targetY;
      // EM_ASM(console.log('Mouse x: ' + $0 + ', y:' + $1), Input.MouseP.x, Input.MouseP.y);
   }

   return true;
}

EM_BOOL
WasmKeyEventCallback(s32 EventType, const EmscriptenKeyboardEvent *Event, void *UserData)
{
   key_ Key = Key_Null;
   // WasmConsoleLog(Event->code);

   if (Event->keyCode == DOM_VK_LEFT) {
      Key = Key_Left;
   } else if (Event->keyCode == DOM_VK_RIGHT) {
      Key = Key_Right;
   } else if (Event->keyCode == DOM_VK_UP) {
      Key = Key_Up;
   } else if (Event->keyCode == DOM_VK_DOWN) {
      Key = Key_Down;
   } else if (Event->keyCode == DOM_VK_SPACE) {
      Key = Key_Space;
   }

   if (EventType == EMSCRIPTEN_EVENT_KEYDOWN) {
      if (Event->repeat) {
         Input.Keys[Event->keyCode].WentDownOrRepeated = true;
      } else {
         Input.Keys[Event->keyCode].WentDown = true;
         Input.Keys[Event->keyCode].WentDownOrRepeated = true;
      }

      Input.Keys[Event->keyCode].Down = true;
   } else if (EventType == EMSCRIPTEN_EVENT_KEYUP) {
      Input.Keys[Event->keyCode].Down = false;
      Input.Keys[Event->keyCode].WentUp = true;
   }

   if (Key == Key_Null) {
      return false;
   }

   return true;
}

u32
WasmGetFileSize(FILE *File)
{
   fseek(File, 0, SEEK_END);
   u32 Size = ftell(File);
   fseek(File, 0, SEEK_SET);

   return Size;
}


bool
WasmReadFile(char *Filename, read_file *Result)
{
   FILE *File = fopen(Filename, "rb");

   if (File) {
      Result->Size = WasmGetFileSize(File);
      Result->Data = (u8 *)malloc(Result->Size);
      fread(Result->Data, 1, Result->Size, File);
      fclose(File);

      return true;
   }

    return false;
}

int main() {
   EmscriptenWebGLContextAttributes Attributes = {};
   emscripten_webgl_init_context_attributes(&Attributes);

   Attributes.majorVersion = 2;
   Attributes.minorVersion = 0;
   Attributes.alpha = true;
   Attributes.depth = true;
   Attributes.antialias = true;
   // Attributes.explicitSwapControl = true;
   Attributes.renderViaOffscreenBackBuffer = true;

   State.WebGLContext = emscripten_webgl_create_context("canvas", &Attributes);

   if (State.WebGLContext <= 0) {
      WasmPrintError((EMSCRIPTEN_RESULT)State.WebGLContext);
   } else {
      WasmConsoleLog("Initializing GL");

      EMSCRIPTEN_RESULT Result = emscripten_webgl_make_context_current(State.WebGLContext);
      WasmPrintError(Result);

      WasmInitOpengl();

      {
         // load textures
         read_file Bride;
         int x,y,n;
         unsigned char *data = stbi_load("/assets/karloff.png", &x, &y, &n, 0);

         EM_ASM(console.log('Image w: ' + $0 + ', h:' + $1), x, y);

         GLuint Texture;
         glGenTextures(1, &Texture);
         glBindTexture(GL_TEXTURE_2D, Texture);

         GLint ImageMode = GL_RGB;
         GLint InternalFormat = GL_RGB8;

         // if (n == 4) {
            ImageMode = GL_RGBA;
            InternalFormat = GL_RGBA8;
         // } else if (n == 1) {
         //    ImageMode = GL_RED;
         //    InternalFormat = GL_RED;
         // }

         glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat,
                      x, y,
                      0, ImageMode, GL_UNSIGNED_BYTE, data);
         DumpGlErrors("Upload texture");

         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
         // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
         // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

         Render.TestTexture = Texture;
      }

      Result = emscripten_set_keydown_callback("#body", 0, false, WasmKeyEventCallback);
      Result = emscripten_set_keyup_callback("#body", 0, false, WasmKeyEventCallback);
      emscripten_set_click_callback("#canvas", 0, false, WasmKeyMouseEventCallback);
      emscripten_set_mousedown_callback("#canvas", 0, false, WasmKeyMouseEventCallback);
      emscripten_set_mouseup_callback("#canvas", 0, false, WasmKeyMouseEventCallback);
      emscripten_set_mousemove_callback("#canvas", 0, false, WasmKeyMouseEventCallback);

      WasmConsoleLog("Starting the main loop");
      emscripten_set_main_loop(WasmMainLoop, 0, 0);
      // this sets the swap interval:
      emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
   }
}