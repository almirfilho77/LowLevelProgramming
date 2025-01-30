#include <windows.h>

HWND MainWindow;

WNDPROC Wndproc;

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
        } break;

        case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");
            PostQuitMessage(0);
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
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
            MSG Msg;
            BOOL MessageReturnValue;
            while ((MessageReturnValue = GetMessage(&Msg, NULL, 0, 0)) != 0)
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

    /*hMainWin = CreateWindow("MainWindClass", 
                            "Main Window", 
                            WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            static_cast<HWND>(NULL),
                            static_cast<HMENU>(NULL),
                            hInstance,
                            static_cast<LPVOID>(NULL));
    
    if (!hMainWin)
    {
        return 0;
    }

    ShowWindow(hMainWin, nShowCmd);
    UpdateWindow(hMainWin);

    MSG msg;
    BOOL bRet;
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            break;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return msg.wParam;
    */
   return 0;
}