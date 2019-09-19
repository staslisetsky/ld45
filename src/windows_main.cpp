#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <GL\gl.h>

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

global_variable LPVOID MainFiber;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "ls_math.h"
#include "render.h"
#include "wasm_keymap.h"
#include "platform.h"

render Render = {};
input Input = {};

static BOOL GlobalRunning = true;

//
//
//

#include "opengl.cpp"
#include "game.cpp"

internal v2i
WinGetWindowDimension(HWND Window)
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

    v2i Dimension = WinGetWindowDimension(Window);

    switch (Message) {
        case WM_TIMER: {
            SwitchToFiber(MainFiber);
        } break;

        case WM_ENTERSIZEMOVE: {
            SetTimer(Window, 0, 1, 0);
        }; break;

        case WM_EXITSIZEMOVE: {
            KillTimer(Window, 0);
        }; break;

        case WM_COMMAND: {
            u32 Data = (u32)WParam;
            // SelectedItem = (context_menu_item)Data;
        } break;

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
            glViewport(0, 0, Dimension.x, Dimension.y);
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

DWORD NextUpdate;

void CALLBACK
MessageFiberProc(input *Input)
{
    while (true) {
        DWORD Now = GetTickCount();

        if (Now >= NextUpdate || MsgWaitForMultipleObjects(0,0,FALSE, NextUpdate - Now, QS_ALLEVENTS) == WAIT_OBJECT_0) {
            MSG Message;
            while (PeekMessageW(&Message, 0, 0, 0, PM_REMOVE)) {
                switch(Message.message) {
                    case WM_QUIT: {
                        GlobalRunning = false;
                    } break;

                    case WM_MOUSEWHEEL: {
                        u32 Data = (u32)Message.wParam;
                        s16 Delta = (s32)((Data & 0xffff0000) >> 16);
                        b32 CtrlKeyPressed = ((Data & MK_CONTROL) == 1);

                        Input->dWheel = Delta;
                    } break;

                    case WM_RBUTTONUP:
                    case WM_RBUTTONDOWN: {
                        b32 Down = (Message.wParam & MK_RBUTTON) != 0;
                        ProcessMouseMessage(Input->MouseButtons + 2, Down);
                    } break;

                    case WM_LBUTTONUP:
                    case WM_LBUTTONDOWN: {
                        b32 Down = (Message.wParam & MK_LBUTTON) != 0;
                        ProcessMouseMessage(Input->MouseButtons, Down);
                    } break;

                    case WM_MBUTTONUP:
                    case WM_MBUTTONDOWN: {
                        b32 Down = (Message.wParam & MK_LBUTTON) != 0;
                        ProcessMouseMessage(Input->MouseButtons + 1, Down);
                    } break;

                    case WM_LBUTTONDBLCLK: {
                        ProcessMouseMessage(Input->MouseButtons, true);
                        Input->MouseButtons[0].DoubleClick = true;
                    } break;

                    case WM_KEYUP:
                    case WM_SYSKEYUP:  {
                        ProcessKeyMessage(Input, Message.lParam, Message.wParam);
                    } break;

                    case WM_KEYDOWN:
                    case WM_SYSKEYDOWN:
                    {
                        ProcessKeyMessage(Input, Message.lParam, Message.wParam);

                        if (Message.wParam != VK_SHIFT &&
                            Message.wParam != VK_CONTROL &&
                            Message.wParam != VK_MENU)
                        {
                            TranslateMessage(&Message);
                            DispatchMessageW(&Message);
                        }
                    } break;

                    case WM_CHAR: {
                        u32 RepeatCount = (Message.lParam & 0x0000ffff);
                        wchar_t Character = (wchar_t)Message.wParam;

                        for (u32 i = 0; i < RepeatCount; ++i) {
                            if (Character >= 0x20 &&
                                (Character < 0x7f || Character > 0xa0))
                            {
                                u32 Size = ls_string::UnicodeCodepointToUtf8((u32)Character,
                                                                             (utf8 *)(Input->UserText.Data + Input->UserText.Size));
                                Assert(Input->UserText.Size + Size <= Input->UserText.Cap);
                                Input->UserText.Size += Size;
                            }
                        }
                    } break;

                    default:
                    {
                        TranslateMessage(&Message);
                        DispatchMessageW(&Message);
                    } break;
                }
            }
        }

        SwitchToFiber(MainFiber);
    }
}

int
WinMain(HINSTANCE Instance, PrevInstance, LPSTR CmdLine, int ShowCmd)
{
   NextUpdate = GetTickCount();

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
          WS_OVERLAPPED | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
          100, 100, 220, 220,
          0, 0, Instance, 0
      );

      if (Window) {
         HDC DeviceContext = GetDC(Window);

         RECT Rect;
         GetClientRect(Window, &Rect);

         UpdateWindow(Window);
         ShowWindow(Window, SW_MAXIMIZE);
      }

      OpenglInit();

      MainFiber = ConvertThreadToFiber(0);
      HANDLE MessageFiber = CreateFiber(0, (LPFIBER_START_ROUTINE)MessageFiberProc, &Input);

      POINT MousePointer;

      GetCursorPos(&MousePointer);
      ScreenToClient(Window, &MousePointer);

      Input.MouseP.x = MousePointer.x;
      Input.MouseP.y = MousePointer.y;

      while (GlobalRunning) {
         Input.dWheel = 0;
         Input.UserText.Size = 0;

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


         SwitchToFiber(MessageFiber);
         GetCursorPos(&MousePointer);

         ScreenToClient(Window, &MousePointer);

         v2 NewMouseP = {(r32)MousePointer.x, (r32)MousePointer.y};
         Input.dPMouse.x = NewMouseP.x - Input.MouseP.x;
         Input.dPMouse.y = NewMouseP.y - Input.MouseP.y;
         Input.MouseP = NewMouseP;

         // Graphene.ScreenDim = Win32GetWindowDimension(Window);

         Game(0.016f);
         Render();
         SwapBuffers(DeviceContext);

         if (Input.Keys[Key_Alt].Down && Input.Keys[Key_F4].WentDown) {
             GlobalRunning = false;
         } else if (Input.Keys[Key_Alt].Down && Input.Keys[Key_W].WentDown) {
             GlobalRunning = false;
         }
      }

  }
}
