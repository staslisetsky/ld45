#include <windows.h>
#include <WinError.h>
#include <stdint.h>
#include <stdio.h>
#include <vector>

#include <GL\gl.h>
#include <al.h>
#include <alc.h>

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
#define Assert(Expression) if(!(Expression)) {*(int *)0=0;}

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

#define LS_STRING_IMPLEMENTATION
enum ls_string_allocator_ {
};
#include "ls_string.h"
ls_string_allocator *ls_stringbuf::AllocatorTable = 0;

#include "windows_keymap.h"
#include "windows_opengl.h"

#include "ls_math.h"
#include "layout.h"
#include "font.h"

#include "render.h"
#include "platform.h"

static BOOL GlobalRunning = true;

static render Render = {};
static input Input = {};

//
//
//

#include "include/stb_vorbis.c"
#include "openal.cpp"
#include "opengl.cpp"
#include "game.cpp"

v2i
GetWindowSize(HWND Window)
{
    v2i Result;

    RECT Rect;
    GetClientRect(Window, &Rect);

    Result.x = Rect.right - Rect.left;
    Result.y = Rect.bottom - Rect.top;

    return Result;
}

LRESULT CALLBACK MainWindowCallback(
    HWND   Window,
    UINT   Message,
    WPARAM WParam,
    LPARAM LParam)
{
    LRESULT Result = 0;

    v2i WindowSize = GetWindowSize(Window);

    switch (Message) {
        case WM_DESTROY: {
            GlobalRunning = false;
        } break;

        case WM_CLOSE: {
            GlobalRunning = false;
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            glViewport(0, 0, WindowSize.x, WindowSize.y);
            EndPaint(Window, &Paint);
            ReleaseDC(Window, DeviceContext);
        } break;
        default: {
            Result = DefWindowProcW(Window, Message, WParam, LParam);
        }
    }

    return(Result);
}

static void
ProcessKeyMessage(input *Input, LPARAM LParam, WPARAM Keycode)
{
    u32 LowWord = (u32)LParam;
    b32 Down = ((LowWord & (1 << 31)) == 0);

    if (Input->Keys[Keycode].Down != Down) {
        if (Input->Keys[Keycode].Down) {
            Input->Keys[Keycode].WentUp = true;
            Input->Keys[Keycode].WentDown = false;
        } else {
            Input->Keys[Keycode].WentUp = false;
            Input->Keys[Keycode].WentDown = true;
            Input->Keys[Keycode].WentDownOrRepeated = true;
        }

        Input->Keys[Keycode].Down = !Input->Keys[Keycode].Down;
    } else if (Input->Keys[Keycode].Down) {
        Input->Keys[Keycode].WentDownOrRepeated = true;
    }
}

static void
ProcessMouseMessage(button *Button, b32 Down)
{
    if (Button->Down != Down) {
        if (Down) {
            Button->WentDown = true;
            Button->WentUp = false;
        } else {
            Button->WentDown = false;
            Button->WentUp = true;
        }

        Button->Down = !Button->Down;
    }
}

u32
GetFileSize(FILE *File)
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
      Result->Size = GetFileSize(File);
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
    printf("[%s] %s\n", Section, Log);
}

void
ProcessMessages()
{
    MSG Message;

    while (PeekMessageW(&Message, 0, 0, 0, PM_REMOVE)) {
        switch (Message.message) {
            case WM_QUIT: {
                GlobalRunning = false;
            } break;

            case WM_MOUSEWHEEL: {
                u32 Data = (u32)Message.wParam;
                s16 Delta = (s32)((Data & 0xffff0000) >> 16);
                b32 CtrlKeyPressed = ((Data & MK_CONTROL) == 1);

                Input.dWheel = Delta;
            } break;

            case WM_RBUTTONUP:
            case WM_RBUTTONDOWN: {
                b32 Down = (Message.wParam & MK_RBUTTON) != 0;
                ProcessMouseMessage(Input.Mouse + 2, Down);
            } break;

            case WM_LBUTTONUP:
            case WM_LBUTTONDOWN: {
                b32 Down = (Message.wParam & MK_LBUTTON) != 0;
                ProcessMouseMessage(Input.Mouse, Down);
            } break;

            case WM_LBUTTONDBLCLK: {
                ProcessMouseMessage(Input.Mouse, true);
                Input.Mouse[0].DoubleClick = true;
            } break;

            case WM_KEYUP:
            case WM_SYSKEYUP:  {
                ProcessKeyMessage(&Input, Message.lParam, Message.wParam);
            } break;

            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            {
                ProcessKeyMessage(&Input, Message.lParam, Message.wParam);

                if (Message.wParam != VK_SHIFT &&
                    Message.wParam != VK_CONTROL &&
                    Message.wParam != VK_MENU)
                {
                    TranslateMessage(&Message);
                    DispatchMessageW(&Message);
                }
            } break;

            default: {
                TranslateMessage(&Message);
                DispatchMessageW(&Message);
            } break;
        }
    }
}

// int
// WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, int ShowCmd)
// {
void
main()
{
    HINSTANCE Instance = GetModuleHandleW(0);

    WNDCLASSW WindowClass = {};
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    WindowClass.lpfnWndProc = MainWindowCallback;
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = L"MainClass";

    if (RegisterClassW(&WindowClass)) {
        HWND Window = CreateWindowExW(
            0,
            WindowClass.lpszClassName,
            L"Ludum Dare - 45",
            WS_OVERLAPPEDWINDOW,
            100, 100, 1000, 500,
            0, 0, Instance, 0
        );

        RECT Rect;
        GetClientRect(Window, &Rect);
        UpdateWindow(Window);
        ShowWindow(Window, SW_SHOW);
        HDC DeviceContext = GetDC(Window);

        InitOpenglContext(Window);
        InitOpengl();
        InitOpenal();

        POINT MousePointer;
        GetCursorPos(&MousePointer);
        ScreenToClient(Window, &MousePointer);

        Input.MouseP.x = MousePointer.x;
        Input.MouseP.y = MousePointer.y;

        Render.Screen = GetWindowSize(Window);
        GameInit();

        HANDLE WatchDir = CreateFileA("shaders", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, 0);

        u32 WatchBufferSize = 4096;
        u32 WatchBytesReceived;
        char *WatchBuffer = (char *)malloc(WatchBufferSize);
        OVERLAPPED Overlapped = {};

        BOOL Cunt = ReadDirectoryChangesW(WatchDir, WatchBuffer, WatchBufferSize, 0, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
                              (LPDWORD)&WatchBytesReceived, &Overlapped, 0);
        DWORD Error = GetLastError();
        HANDLE WatchIOCP = CreateIoCompletionPort(WatchDir, 0, 0, 0);

        while (GlobalRunning) {
            OVERLAPPED *O;
            DWORD Bytes;
            ULONG_PTR Key;

            if (GetQueuedCompletionStatus(WatchIOCP, &Bytes, &Key, &O, 1)) {
                char *Data = WatchBuffer;
                u32 Next = 0;

                do {
                    FILE_NOTIFY_INFORMATION *Info = (FILE_NOTIFY_INFORMATION *)Data;
                    Next = Info->NextEntryOffset;

                    char AsciiFilename[256];
                    wcstombs (AsciiFilename, Info->FileName, Info->FileNameLength / 2);

                    u32 CharCount = Info->FileNameLength / 2;
                    u32 LastDot = CharCount - 1;

                    for (u32 i=0; i<CharCount; ++i) {
                        if (Info->FileName[CharCount - i - 1] == L'.') {
                           break;
                        }
                        --LastDot;
                    }

                    ls_stringbuf ShaderName;
                    ShaderName.AppendCStringN(AsciiFilename, LastDot);

                    for (u32 i=0; i<Shader_Count; ++i) {
                        if (Render.Shaders[i].Name == ShaderName) {
                            glDeleteProgram(Render.Shaders[i].Id);
                            Render.Shaders[i].Id = LoadShader(Render.Shaders[i].Name.Data);
                        }
                    }

                    Data += Next;
                } while (Next);

                ReadDirectoryChangesW(WatchDir, WatchBuffer, WatchBufferSize, 0, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
                                      (LPDWORD)&WatchBytesReceived, &Overlapped, 0);
            }

            Input.dWheel = 0;

            for (u32 KeyIndex = 0; KeyIndex < Key_Count; ++KeyIndex) {
                Input.Keys[KeyIndex].WentDown = 0;
                Input.Keys[KeyIndex].WentUp = 0;
                Input.Keys[KeyIndex].WentDownOrRepeated = 0;
            }

            Input.Mouse[0].WentDown = 0;
            Input.Mouse[0].WentUp = 0;
            Input.Mouse[0].DoubleClick = false;
            Input.Mouse[1].WentDown = 0;
            Input.Mouse[1].WentUp = 0;
            Input.Mouse[1].DoubleClick = false;

            GetCursorPos(&MousePointer);
            ScreenToClient(Window, &MousePointer);

            v2 NewMouseP = {(r32)MousePointer.x, (r32)MousePointer.y};
            Input.dPMouse.x = NewMouseP.x - Input.MouseP.x;
            Input.dPMouse.y = NewMouseP.y - Input.MouseP.y;
            Input.MouseP = NewMouseP;

            Render.PlainVertexCount = 0;
            Render.TexturedVertexCount = 0;
            Render.CommandCount = 0;

            ProcessMessages();
            Render.Screen = GetWindowSize(Window);
            Game(0.016f);
            OpenglRender(Render);
            SwapBuffers(DeviceContext);


            if (Input.Keys[Key_Alt].Down && Input.Keys[Key_F4].WentDown) {
                GlobalRunning = false;
            } else if (Input.Keys[Key_Esc].Down) {
                GlobalRunning = false;
            }
        }
    }
}
