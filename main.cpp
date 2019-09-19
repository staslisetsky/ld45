#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <string.h>

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
   glGenBuffers(1, &Render.VertexBufferPlain);

   // Plain vertex
   glBindVertexArray(Render.VertexArrayPlain);
   glBindBuffer(GL_ARRAY_BUFFER, Render.VertexBufferPlain);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_xyzrgba) * 1000, 0, GL_DYNAMIC_DRAW);
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgba), (GLvoid *)0);
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgba), (GLvoid *)12);

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

   Render.PlainShader = CreateProgram(PlainV, PlainF);
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

   DrawRect(&Render, v4{1.0f,0.5f,0.5f,1.0f}, v2{X, Y}, v2{100.0f, 100.0f}, 1);
   RenderCommands(Render);

   // swap:
   // emscripten_webgl_commit_frame();
   Render.PlainVertexCount = 0;
   Render.TexturedVertexCount = 0;
   Render.CommandCount = 0;

   ++State.Frame;
}

EM_BOOL
WasmKeyEventCallback(s32 EventType, const EmscriptenKeyboardEvent *Event, void *UserData)
{
   key_ Key = Key_Null;
   WasmConsoleLog(Event->code);

   if (strcmp(Event->code, "ArrowLeft") == 0) {
      Key = Key_Left;
   } else if (strcmp(Event->code, "ArrowRight") == 0) {
      Key = Key_Right;
   } else if (strcmp(Event->code, "ArrowUp") == 0) {
      Key = Key_Up;
   } else if (strcmp(Event->code, "ArrowDown") == 0) {
      Key = Key_Down;
   } else if (strcmp(Event->code, "Space") == 0) {
      Key = Key_Space;
   }

   if (EventType == EMSCRIPTEN_EVENT_KEYDOWN) {
      if (Event->repeat) {
         Input.Keys[Key].WentDownOrRepeated = true;
      } else {
         Input.Keys[Key].WentDown = true;
         Input.Keys[Key].WentDownOrRepeated = true;
      }

      Input.Keys[Key].Down = true;
   } else if (EventType == EMSCRIPTEN_EVENT_KEYUP) {
      Input.Keys[Key].Down = false;
      Input.Keys[Key].WentUp = true;
   }

   if (Key == Key_Null) {
      return false;
   }

   return true;
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

      Result = emscripten_set_keydown_callback("#body", 0, false, WasmKeyEventCallback);
      Result = emscripten_set_keyup_callback("#body", 0, false, WasmKeyEventCallback);

      WasmConsoleLog("Starting the main loop");
      emscripten_set_main_loop(WasmMainLoop, 0, 0);
      // this sets the swap interval:
      emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
   }
}