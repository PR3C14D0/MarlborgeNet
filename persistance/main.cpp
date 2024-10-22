#include "pragma.h"
#include <windows.h>

void Main()
{
    MessageBoxA(0, "Dll loaded", "Meow", MB_OK);
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        MessageBoxA(0, "Dll loaded", "Meow", MB_OK);
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}