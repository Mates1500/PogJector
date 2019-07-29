#include <Windows.h>
#include <string>
#include <vector>
#include <functional>
#include "Win32Handle.h"

#pragma once

typedef std::unique_ptr<void, std::function<void(void*)>> ProcessMemoryAllocator;

class Process
{
public:
    Process(std::string process_name);
    
    LPVOID AllocateMemory(size_t size);
    ProcessMemoryAllocator GetMemoryAllocator(size_t size);
    void FreeMemory(LPVOID address);
    void WriteMemory(LPVOID address, LPCVOID data, size_t size) const;
    DWORD CallFunction(LPCVOID address, LPVOID arg);

    int GetProcessId() const;
private:
    int processId;
    Win32Handle m_handle;

    std::vector<LPVOID> m_allocations;

    static int GetProcessIdByName(const char* process_name);
};



