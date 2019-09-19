#include <emscripten/emscripten.h>
#include <emscripten/key_codes.h>
#include <emscripten/html5.h>

#include <stdint.h>
#include <malloc.h>

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

#include "ls_math.h"
#include "render.h"
#include "wasm_keymap.h"
#include "platform.h"

struct wasm_state {
   EMSCRIPTEN_WEBGL_CONTEXT_HANDLE WebGLContext;
   u32 Frame;
};

wasm_state State = {};
render Render = {};
input Input = {};

//
//
//

#include "opengl.cpp"
#include "game.cpp"

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
   if ((Event->keyCode >= 0x30 && Event->keyCode <= 0x39) ||
       (Event->keyCode >= 0x41 && Event->keyCode <= 0x5a) ||
       (Event->keyCode >= 0x0E && Event->keyCode <= 0x2E))
   {
      key_ Key = (key_)Event->keyCode;

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

      return true;
   }

   return false;
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

      OpenglInit();

      {
         image Image;
         Image.Data = stbi_load("/assets/karloff.png", (s32 *)&Image.Width, (s32 *)&Image.Height, (s32 *)&Image.N, 0);
         EM_ASM(console.log('Loaded Image w: ' + $0 + ', h:' + $1), Image.Width, Image.Height);
         Render.TestTexture = OpenglUploadTexture(Image);
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