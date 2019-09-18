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

#include "ls_math.h"

struct vertex_xyzrgba {
    v3 P;
    v4 Color;
};

struct vertex_xyzrgbauv {
    v3 P;
    v4 Color;
    v2 UV;
};

struct wasm_state {
   EMSCRIPTEN_WEBGL_CONTEXT_HANDLE WebGLContext;
   GLuint VertexArrayPlain;
   GLuint VertexBufferPlain;

   GLuint PlainShader;

   b32 Initialized;

   u32 Frame;
};

wasm_state State = {};

void
WasmConsoleLog(const char *String)
{
   EM_ASM(console.log(UTF8ToString($0)), String);
}

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
   glGenBuffers(1, &State.VertexArrayPlain);
   glGenBuffers(1, &State.VertexBufferPlain);

   // Plain vertex
   glBindVertexArray(State.VertexArrayPlain);
   glBindBuffer(GL_ARRAY_BUFFER, State.VertexBufferPlain);
   glBufferData(GL_ARRAY_BUFFER, 1024 * 1024, NULL, GL_DYNAMIC_DRAW);
   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgba), (GLvoid *)0);
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex_xyzrgba), (GLvoid *)12);

   // layout(std140) uniform view\n
   // {\n
   //     mat4 Projection;\n
   // } View;\n

   const char *PlainV = R"str(
   attribute vec4 In_P;
   attribute vec4 In_VertexColor;
   varying   vec4 VertexColor;

   void main()
   {
       gl_Position = In_P;
       VertexColor = In_VertexColor;
   }
)str";

   const char *PlainF = R"str(
precision mediump float;
varying vec4 VertexColor;
void main(void)
{
   gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
)str";

   State.PlainShader = CreateProgram(PlainV, PlainF);


   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void
WasmMainLoop()
{
   r32 Red = (sin((r32)State.Frame / 100.0f) + 1.0f) / 2.0f;
   r32 Green = (sin((r32)State.Frame / 200.0f) + 1.0f) / 2.0f;
   r32 Blue = (cos((r32)State.Frame / 300.0f) + 1.0f) / 2.0f;

   glClearColor(Red, Green, Blue, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT);

   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glClear(GL_DEPTH_BUFFER_BIT);
   // WasmConsoleLog("Loop");

   // swap:
   // emscripten_webgl_commit_frame();

   ++State.Frame;
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

      WasmConsoleLog("Starting the main loop");
      emscripten_set_main_loop(WasmMainLoop, 0, 0);
      // this sets the swap interval:
      emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
   }
}