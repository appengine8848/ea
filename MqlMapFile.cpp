#include <windows.h>
#include <iostream>

#define EXPORT __declspec(dllexport)

HANDLE hMapFile = NULL;
LPVOID pBuf = NULL;
const char* szName = "Global\\MyTradeSignal"; // 内存映射文件名
const int BUF_SIZE = 1024; // 缓冲区大小

extern "C" {
    // 打开/创建内存映射文件
    EXPORT bool OpenFileMappingA() {
        if (hMapFile) return true;

        hMapFile = CreateFileMappingA(
            INVALID_HANDLE_VALUE,    // use paging file
            NULL,                    // default security
            PAGE_READWRITE,          // read/write access
            0,                       // maximum object size (high-order DWORD)
            BUF_SIZE,                // maximum object size (low-order DWORD)
            szName);                 // name of mapping object

        if (hMapFile == NULL) {
            return false;
        }

        pBuf = MapViewOfFile(hMapFile,   // handle to map object
            FILE_MAP_ALL_ACCESS, // read/write permission
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

    // 写入数据
    EXPORT bool WriteToMapFileA(const char* data) {
        if (!pBuf) return false;
        CopyMemory((PVOID)pBuf, data, strlen(data) + 1);
        return true;
    }

    // 读取数据
    EXPORT bool ReadFromMapFileA(char* data, int len) {
        if (!pBuf) return false;
        if (len > BUF_SIZE) len = BUF_SIZE;
        CopyMemory((PVOID)data, pBuf, len);
        return true;
    }
}

// DLL入口函数
BOOL APIENTRY DllMain(HMODULE hModule,
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
        if (pBuf) UnmapViewOfFile(pBuf);
        if (hMapFile) CloseHandle(hMapFile);
        break;
    }
    return TRUE;
}
