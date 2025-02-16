#include <windows.h>
#include <stdio.h>
#include <stdint.h>

#define internal static
#define global_variable static
#define local_persist static

global_variable bool Running = false;
global_variable HWND MainWindow;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;

global_variable const int BytesPerPixel = 4;

typedef struct Point {
    int X;
    int Y;
} Point;

internal void Win32DrawSolidColor(void* bitmapMemory, int width, int height, uint8_t red, uint8_t green, uint8_t blue)
{
    uint32_t* PixelValue = static_cast<uint32_t*>(bitmapMemory);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {

            uint8_t* PixelValueByte = reinterpret_cast<uint8_t *>(PixelValue + (i * width + j));
            *(PixelValueByte+0) = blue; //Padding -> Blue
            *(PixelValueByte+1) = green; //Red -> Green
            *(PixelValueByte+2) = red; //Green -> Red
            *(PixelValueByte+3) = 0x00; //Blue -> Padding
        }
    }
}

internal void Win32DrawSolidColor(void* bitmapMemory, int width, int height, uint32_t color = 0xFFFFFFFF)
{
    uint32_t* PixelValue = static_cast<uint32_t*>(bitmapMemory);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            PixelValue[i * width + j] = color; //XXRRGGBB
        }
    }
}

internal Point Win32GetRectCenter(const RECT &Rectangle)
{
    int X = static_cast<int>((Rectangle.right - Rectangle.left)/2.0);
    int Y = static_cast<int>((Rectangle.bottom - Rectangle.top)/2.0);
    return Point{ X, Y };
}

// DIB stands for Device Independent Bitmap
internal void Win32ResizeDIBSection(int width, int height)
{
    char s[32];
    sprintf(s, "width=%d/height=%d\n", width, height);
    OutputDebugStringA(s);

    if (BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidth = width;
    BitmapHeight = height;

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    BitmapMemory = VirtualAlloc(NULL, BitmapWidth * BitmapHeight * BytesPerPixel, MEM_COMMIT, PAGE_READWRITE);
}

internal void Win32UpdateWindow(HDC deviceContext, const RECT *WindowRect, int x, int y, int width, int height)
{
    int WindowWidth = WindowRect->right - WindowRect->left;
    int WindowHeight = WindowRect->bottom - WindowRect->top;

    // Win32DrawSolidColor(BitmapMemory, BitmapWidth, BitmapHeight, 0x00FF0000);
    Win32DrawSolidColor(BitmapMemory, BitmapWidth, BitmapHeight, 126, 77, 179);

    StretchDIBits(deviceContext, 
        // x, y, width, height,
        // x, y, width, height,
        0, 0, BitmapWidth, BitmapHeight,
        0, 0, WindowWidth, WindowHeight,
        BitmapMemory,
        &BitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

LRESULT WindowMessageCallback(
    HWND windowHandle,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    LRESULT Result = 0;

    switch(msg)
    {
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
            RECT ClientAreaRect;
            GetClientRect(windowHandle, &ClientAreaRect);

            int Height = ClientAreaRect.bottom - ClientAreaRect.top;
            int Width = ClientAreaRect.right - ClientAreaRect.left;

            Win32ResizeDIBSection(Width, Height);

            // UINT width = LOWORD(lParam);
            // UINT height = HIWORD(lParam);
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
            OutputDebugStringA("WM_PAINT\n");
            
            PAINTSTRUCT Paint;
            Paint.fErase = 1;
            HDC DeviceContext = BeginPaint(windowHandle, &Paint);
            
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            
            RECT ClientAreaRect;
            GetClientRect(windowHandle, &ClientAreaRect);
            
            Win32UpdateWindow(DeviceContext, &ClientAreaRect, X , Y, Width, Height);
            EndPaint(windowHandle, &Paint);
        } break;

        default:
        {
            Result = DefWindowProcA(windowHandle, msg, wParam, lParam);
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