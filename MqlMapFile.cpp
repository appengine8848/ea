#include <windows.h>
#include <iostream>

// 定义导出宏和调用约定宏
#define EXPORT __declspec(dllexport)
#define STDCALL __stdcall // 明确定义 STDCALL 宏

HANDLE hMapFile = NULL;
LPVOID pBuf = NULL;
const char* szName = "Global\\MyTradeSignal";
const int BUF_SIZE = 1024;

extern "C" {
    // 【已修正】为每个导出函数添加 STDCALL 调用约定
    EXPORT bool STDCALL Mql_OpenFileMapping() {
        if (hMapFile) return true;

        hMapFile = CreateFileMappingA(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            BUF_SIZE,
            szName);

        if (hMapFile == NULL) {
            return false;
        }

        pBuf = MapViewOfFile(hMapFile,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            BUF_SIZE);

        if (pBuf == NULL) {
            CloseHandle(hMapFile);
            hMapFile = NULL;
            return false;
        }
        return true;
    }

    // 【已修正】为每个导出函数添加 STDCALL 调用约定
    EXPORT bool STDCALL Mql_WriteToMapFile(const char* data) {
        if (!pBuf) return false;
        CopyMemory((PVOID)pBuf, data, strlen(data) + 1);
        return true;
    }

    // 【已修正】为每个导出函数添加 STDCALL 调用约定
    EXPORT bool STDCALL Mql_ReadFromMapFile(char* data, int len) {
        if (!pBuf) return false;
        if (len > BUF_SIZE) len = BUF_SIZE;
        CopyMemory((PVOID)data, pBuf, len);
        return true;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (pBuf) UnmapViewOfFile(pBuf);
        if (hMapFile) CloseHandle(hMapFile);
        break;
    }
    return TRUE;
}
