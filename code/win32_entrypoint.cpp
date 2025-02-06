#include <windows.h>

static bool Running = false;
HWND MainWindow;

typedef struct Point {
    int X;
    int Y;
} Point;

static Point s_GetRectCenter(const RECT &rectangle)
{
    int X = static_cast<int>((rectangle.right - rectangle.left)/2.0);
    int Y = static_cast<int>((rectangle.bottom - rectangle.top)/2.0);
    return Point{ X, Y };
}

LRESULT WindowMessageCallback(
  HWND WindowHandle,
  UINT Msg,
  WPARAM WParam,
  LPARAM LParam
)
{
    LRESULT Result = 0;

    switch(Msg)
    {
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
            UINT width = LOWORD(LParam);
            UINT height = HIWORD(LParam);
            //std::cout << "New size is " << width << "x" << height << '\n';
        } break;

        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
            Running = false;
        } break;

        case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");
            Running = false;
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint = {};
            Paint.fErase = 1;
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.right;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            HDC DeviceContext = BeginPaint(WindowHandle, &Paint);
            if (DeviceContext == 0)
            {
                break;
            }
            else
            {
                Point centerPoint = s_GetRectCenter(Paint.rcPaint);
                int centerX = centerPoint.X;
                int centerY = centerPoint.Y;
                TextOutA(DeviceContext, centerX, centerY, "X", 1);
                PatBlt(DeviceContext, X, Y, Width, Height, WHITENESS);
                EndPaint(WindowHandle, &Paint);
            }
        } break;

        default:
        {
            Result = DefWindowProcA(WindowHandle, Msg, WParam, LParam);
        } break;
    }

    return Result;
}

int APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd
)
{
    WNDCLASSA MainWindowClass = {0};
    MainWindowClass.style = CS_OWNDC|CS_VREDRAW|CS_HREDRAW;
    MainWindowClass.lpfnWndProc = WindowMessageCallback;
    MainWindowClass.hInstance = hInstance;
    // MainWindowClass.hIcon = ;
    MainWindowClass.lpszClassName = "MainWindowClassName";

    if (RegisterClassA(&MainWindowClass))
    {
        MainWindow = CreateWindowExA(0, 
                                     MainWindowClass.lpszClassName,
                                     "Main Window",
                                     WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     CW_USEDEFAULT, CW_USEDEFAULT,
                                     static_cast<HWND>(NULL),
                                     static_cast<HMENU>(NULL),
                                     hInstance,
                                     static_cast<LPVOID>(NULL)
                                     );
        
        if (MainWindow)
        {
            Running = true;
            MSG Msg;
            BOOL MessageReturnValue;
            while (Running && (MessageReturnValue = GetMessage(&Msg, NULL, 0, 0)) != 0)
            {
                if (MessageReturnValue > 0)
                {
                    TranslateMessage(&Msg);
                    DispatchMessageA(&Msg);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            //logging
        }
    }
    else
    {
        //logging
    }
    
   return 0;
}