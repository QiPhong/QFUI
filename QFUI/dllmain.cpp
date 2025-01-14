﻿// dllmain.cpp : 定义 DLL 应用程序的入口点。

#include "pch.h"
#include <stdio.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

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


extern "C"
__declspec(dllexport) void PreventScreenshot(HWND hwnd)
{
    SetWindowDisplayAffinity(hwnd,WDA_MONITOR);
}
extern "C"
__declspec(dllexport) void AllowScreenshot(HWND hwnd)
{
    SetWindowDisplayAffinity(hwnd, WDA_NONE);
}
