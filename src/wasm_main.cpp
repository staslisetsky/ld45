#include <emscripten/emscripten.h>
#include <emscripten/key_codes.h>
#include <emscripten/html5.h>

#include <stdint.h>
#include <unistd.h>
#include <malloc.h>
#include <vector>

#include <GLES3/gl3.h>
#include <AL/al.h>
#include <AL/alc.h>

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

#define ArrayCount(Array) ((sizeof(Array)) / (sizeof(Array[0])))
#define Assert(Expression) if(!(Expression)) { EM_ASM(console.log('Assertion failed ' + UTF8ToString($0) + ': ' + $1), __FUNCTION__, __LINE__); *(int *)0=0; }

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

#define LS_STRING_IMPLEMENTATION
enum ls_string_allocator_ {
};
#include "ls_string.h"
ls_string_allocator *ls_stringbuf::AllocatorTable = 0;

#include "wasm_keymap.h"
#include "ls_math.h"
#include "layout.h"
#include "font.h"

struct wasm_state {
   EMSCRIPTEN_WEBGL_CONTEXT_HANDLE WebGLContext;
   u32 Frame;
   r64 LastFrameMs;
};

#include "render.h"
#include "platform.h"

wasm_state WasmState = {};
input Input = {};

//
//
//

void
WasmConsoleLog(const char *String)
{
   EM_ASM(console.log(UTF8ToString($0)), String);
}

#include "include/stb_vorbis.c"
#include "openal.cpp"
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
   r64 Time = emscripten_performance_now();
   r32 dT = Time - WasmState.LastFrameMs;
   WasmState.LastFrameMs = Time;

   for (u32 i=0; i<Key_Count; ++i) {
      Input.Keys[i].WentDown = false;
      Input.Keys[i].WentUp = false;
      Input.Keys[i].WentDownOrRepeated = false;
   }

   Input.Mouse[0].WentDown = false;
   Input.Mouse[0].WentUp = false;
   Input.Mouse[1].WentDown = false;
   Input.Mouse[1].WentUp = false;

   u32 LastEvent = Input.LastEvent;
   while (Input.LastUsedEvent != LastEvent) {
      u32 FirstUnusedEvent = (Input.LastUsedEvent + 1) % MAX_INPUT_EVENTS;

      input_event *Event = Input.Events + FirstUnusedEvent;

      if (Event->Type == InputEvent_MouseDown) {
         Input.Mouse[Event->Index].WentDown = true;
      } else if (Event->Type == InputEvent_MouseUp) {
         Input.Mouse[Event->Index].WentUp = true;
      } else if (Event->Type == InputEvent_KeyDown) {
         EM_ASM(console.log("Key Down: " + $0 + " Index: " + $1), Event->Index, FirstUnusedEvent);

         Input.Keys[Event->Index].Down = true;
         Input.Keys[Event->Index].WentDown = true;
         Input.Keys[Event->Index].WentDownOrRepeated = true;
      } else if (Event->Type == InputEvent_KeyDownRepeated) {
         Input.Keys[Event->Index].WentDownOrRepeated = true;
      } else if (Event->Type == InputEvent_KeyUp) {
         EM_ASM(console.log("Key Up: " + $0 + " Index: " + $1), Event->Index, FirstUnusedEvent);
         Input.Keys[Event->Index].Down = false;
         Input.Keys[Event->Index].WentUp = true;
      }

      Input.LastUsedEvent = FirstUnusedEvent;
   }

   Render.Screen.x = 1000;
   Render.Screen.y = 500;

   Game(dT / 1000.0f);
   OpenglRender(Render);

   Render.PlainVertexCount = 0;
   Render.TexturedVertexCount = 0;
   Render.CommandCount = 0;
}

EM_BOOL
WasmKeyMouseEventCallback(s32 EventType, const EmscriptenMouseEvent *Event, void *UserData)
{
   u32 Button = 0;
   if (Event->button == 2) {
      Button = 1;
   }

   input_event InputEvent;
   u32 EventIndex = (Input.LastEvent + 1) % MAX_INPUT_EVENTS;

   if (EventType == EMSCRIPTEN_EVENT_MOUSEDOWN) {
      InputEvent.Type = InputEvent_MouseDown;
      InputEvent.Index = Button;

      Input.Events[EventIndex] = InputEvent;
      Input.LastEvent = EventIndex;
   } else if (EventType == EMSCRIPTEN_EVENT_MOUSEUP) {
      InputEvent.Type = InputEvent_MouseUp;
      InputEvent.Index = Button;

      Input.Events[EventIndex] = InputEvent;
      Input.LastEvent = EventIndex;
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
      input_event InputEvent = {};
      u32 EventIndex = (Input.LastEvent + 1) % MAX_INPUT_EVENTS;

      key_ Key = (key_)Event->keyCode;
      InputEvent.Index = Key;

      if (EventType == EMSCRIPTEN_EVENT_KEYDOWN) {
         if (Event->repeat) {
            InputEvent.Type = InputEvent_KeyDownRepeated;
         } else {
            EM_ASM(console.log("Callback Event: Down: " + $0 + ", Index: " + $1), Key, EventIndex);
            InputEvent.Type = InputEvent_KeyDown;
         }
      } else if (EventType == EMSCRIPTEN_EVENT_KEYUP) {
         EM_ASM(console.log("Callback Event: Up: " + $0 + ", Index: " + $1), Key, EventIndex);
         InputEvent.Type = InputEvent_KeyUp;
      }

      Input.Events[EventIndex] = InputEvent;
      Input.LastEvent = EventIndex;

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

bool os::
ReadFile(char *Filename, read_file *Result)
{
   FILE *File = fopen(Filename, "rb");

   if (File) {
      Result->Size = WasmGetFileSize(File);
      Result->Data = (u8 *)malloc(Result->Size + 1);
      fread(Result->Data, 1, Result->Size, File);
      fclose(File);

      Result->Data[Result->Size] = 0;

      return true;
   }

   return false;
}

void os::
PrintLog(char *Section, char *Log)
{
   EM_ASM(console.log('[' + UTF8ToString($0) + '] ' + $1), Section, Log);
}

int main() {
   EmscriptenWebGLContextAttributes Attributes = {};
   emscripten_webgl_init_context_attributes(&Attributes);

   Attributes.majorVersion = 2;
   Attributes.minorVersion = 0;
   Attributes.alpha = true;
   Attributes.depth = true;
   Attributes.antialias = true;

   WasmState.WebGLContext = emscripten_webgl_create_context("canvas", &Attributes);

   if (WasmState.WebGLContext <= 0) {
      WasmPrintError((EMSCRIPTEN_RESULT)WasmState.WebGLContext);
   } else {
      WasmConsoleLog("Initializing GL");

      EMSCRIPTEN_RESULT Result = emscripten_webgl_make_context_current(WasmState.WebGLContext);
      WasmPrintError(Result);

      chdir("/data");

      WasmState.LastFrameMs = emscripten_performance_now();

      InitOpengl();
      InitOpenal();

      Render.Screen.x = 1000;
      Render.Screen.y = 500;
      GameInit();

      Result = emscripten_set_keydown_callback("#body", 0, false, WasmKeyEventCallback);
      Result = emscripten_set_keyup_callback("#body", 0, false, WasmKeyEventCallback);
      emscripten_set_click_callback("#canvas", 0, false, WasmKeyMouseEventCallback);
      emscripten_set_mousedown_callback("#canvas", 0, false, WasmKeyMouseEventCallback);
      emscripten_set_mouseup_callback("#canvas", 0, false, WasmKeyMouseEventCallback);
      emscripten_set_mousemove_callback("#canvas", 0, false, WasmKeyMouseEventCallback);

      emscripten_run_script("gameInitialized()");

      WasmConsoleLog("Starting the main loop");
      emscripten_set_main_loop(WasmMainLoop, 0, 0);

      // this sets the swap interval:
      emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
   }
}