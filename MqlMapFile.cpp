#include <windows.h>

#define EXPORT __declspec(dllexport)
#define STDCALL __stdcall

HANDLE hMapFile = NULL;
LPVOID pBuf = NULL;
// 【修正】使用宽字符定义文件名
const wchar_t* szName = L"Global\\MyTradeSignal_Unicode";
const int BUF_SIZE = 1024; // 1024 bytes = 512 wide characters

extern "C" {
    EXPORT bool STDCALL Mql_OpenFileMapping() {
        if (hMapFile) return true;
        // 【修正】使用 CreateFileMappingW
        hMapFile = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE, szName);
        if (hMapFile == NULL) return false;
        pBuf = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
        if (pBuf == NULL) {
            CloseHandle(hMapFile);
            hMapFile = NULL;
            return false;
        }
        return true;
    }

    // 【修正】接收宽字符指针 const wchar_t*
    EXPORT bool STDCALL Mql_WriteToMapFile(const wchar_t* data) {
        if (!pBuf) return false;
        // 【修正】使用 lstrcpyW
        lstrcpyW((wchar_t*)pBuf, data);
        return true;
    }

    // 【修正】写入宽字符指针 wchar_t*
    EXPORT bool STDCALL Mql_ReadFromMapFile(wchar_t* data, int len) {
        if (!pBuf) return false;
        // 【修正】使用 lstrcpynW。注意len是字符数，不是字节数
        lstrcpynW(data, (const wchar_t*)pBuf, len);
        return true;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_DETACH:
        if (pBuf) UnmapViewOfFile(pBuf);
        if (hMapFile) CloseHandle(hMapFile);
        break;
    }
    return TRUE;
}
