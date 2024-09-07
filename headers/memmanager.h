#pragma once

#include <windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <stdint.h>
#include <stdio.h>

typedef struct _MODULE_INFO {
    HANDLE hMod;
    uint64_t base;
    uintptr_t size;
}MODULE_INFO, *PMODULE_INFO;

class MemoryManager {
public:
    DWORD pID;
    HANDLE hProc;
    MODULE_INFO mInfo;

    template<class T>
    T ReadMem(uintptr_t address) {
        T buffer;
        ReadProcessMemory(hProc, (LPVOID)address, (LPVOID)&buffer,  sizeof(T), 0);
        return buffer;
    }

    bool ReadBytes(uintptr_t address, size_t size, _Out_ LPVOID buffer) {
        SIZE_T bytesread;
        return ReadProcessMemory(hProc, (LPVOID)address, buffer, size, &bytesread) && bytesread == size;
    }

    HANDLE getProcess(const wchar_t* name) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);
        while (Process32Next(snapshot, &processEntry)) {
            if (!_wcsicmp(processEntry.szExeFile, name)) {
                pID = processEntry.th32ProcessID;
                hProc = OpenProcess(PROCESS_ALL_ACCESS, false, pID);
                break;
            }
        }
        CloseHandle(snapshot);
        return hProc;
    }

    HANDLE getModule(const wchar_t* name) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pID);
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(MODULEENTRY32);

        while(Module32Next(snapshot, &modEntry)) {
            if (!_wcsicmp(modEntry.szModule, name)) {
                mInfo.hMod = modEntry.hModule;
                mInfo.base = (uint64_t)modEntry.modBaseAddr;
                mInfo.size = modEntry.dwSize;
                break;
            }
        }
        CloseHandle(snapshot);
        return mInfo.hMod;
    }

};