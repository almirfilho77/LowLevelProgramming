#include <windows.h>

int APIENTRY WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nShowCmd
)
{
    return MessageBoxA(NULL, "Click OK to close", "My Cool Window", MB_OK|MB_ICONINFORMATION);
}