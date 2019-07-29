#include "Process.h"
#include <TlHelp32.h>
#include <stdexcept>
#include <string>

Process::Process(std::string process_name)
    :processId(GetProcessIdByName(process_name.c_str())),
    m_handle(OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, processId))
{
    if (m_handle == nullptr)
        throw std::runtime_error("Process:Process - Failed to open process with id " + std::to_string(processId) + ", error id:" + std::to_string(GetLastError()));
}

ProcessMemoryAllocator Process::GetMemoryAllocator(size_t size)
{


    return std::unique_ptr<void, std::function<void(void*)>>(
        VirtualAllocEx(m_handle.get(), NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)
        , [handle = m_handle.get()](void* ptr)
    {
        if (handle == nullptr) return;

    	if (!VirtualFreeEx(handle, ptr, 0, MEM_RELEASE))
    		throw std::runtime_error("Utility::Process::AllocMemoryFailed to free allocated memory! Handle already closed?");
    });
}

LPVOID Process::AllocateMemory(size_t size)
{
    LPVOID address = VirtualAllocEx(m_handle.get(), NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    if (address == nullptr)
        throw std::runtime_error("Process:AllocateMemory - Memory allocation failed");

    // TODO: try to implement this for automated smart-pointer RAII
	//return std::unique_ptr<void, std::function<void(void*)>>(alloc, [handle = m_handle.get()](void* ptr)
	//{
	//	if (!VirtualFreeEx(handle, ptr, 0, MEM_RELEASE))
	//		throw std::runtime_error("Utility::Process::AllocMemoryFailed to free allocated memory! Handle already closed?");
	//});

    m_allocations.push_back(address);
    return address;
}

void Process::FreeMemory(LPVOID address)
{
    auto foundResultIterator = std::find(m_allocations.begin(), m_allocations.end(), address);
    if (foundResultIterator == m_allocations.end())
        throw std::runtime_error("Process:FreeMemory - No allocations at the specified address could be found");
    m_allocations.erase(foundResultIterator);

    bool freeSuccess = VirtualFreeEx(m_handle.get(), address, 0, MEM_RELEASE);

    if (!freeSuccess)
        throw std::runtime_error("Process:FreeMemory - VirtualFreeEx failed, error id:" + std::to_string(GetLastError()));
}

void Process::WriteMemory(LPVOID address, LPCVOID data, size_t size) const
{
    SIZE_T bytesWritten = 0;
    bool writeSuccess = WriteProcessMemory(m_handle.get(), address, data, size, &bytesWritten);

    if (!writeSuccess)
        throw std::runtime_error("Process:WriteMemory - WriteProcessMemory failed, error id:" + std::to_string(GetLastError()));
    if (bytesWritten != size)
        throw std::runtime_error("Process:WriteMemory - Data has been written only partially");
}

DWORD Process::CallFunction(LPCVOID address, LPVOID arg)
{
    HANDLE thread = CreateRemoteThread(m_handle.get(), 0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(address), arg, 0, 0);
    if (!thread)
        throw std::runtime_error("Process:CallFunction - Error while creating a thread, error id:" + std::to_string(GetLastError()));
    DWORD threadState = WaitForSingleObject(thread, 5000);
    if (threadState != WAIT_OBJECT_0)
        throw std::runtime_error("Process:CallFunction - Error while executing the thread function - timeout");

    DWORD exitCode = 0;
    GetExitCodeThread(thread, &exitCode);
    if (exitCode == 0)
        throw std::runtime_error("Process:CallFunction - Error while executing the thread function - exit code equals 0");
    return exitCode;
}

int Process::GetProcessId() const
{
    return processId;
}

int Process::GetProcessIdByName(const char* process_name)
{
    Win32Handle toolhelpSnapshot = Win32Handle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
    PROCESSENTRY32 procSnapshot = { 0 };

    procSnapshot.dwSize = sizeof(PROCESSENTRY32); // without initializing this, Process32First will fail

    bool nextProcessAvailable = false;
    int processId = 0;
    nextProcessAvailable = Process32First(toolhelpSnapshot.get(), &procSnapshot);
    while (nextProcessAvailable)
    {
        if (strcmp(procSnapshot.szExeFile, process_name) == 0)
        {
            processId = procSnapshot.th32ProcessID;
            break;
        }
        nextProcessAvailable = Process32Next(toolhelpSnapshot.get(), &procSnapshot);
    }
    
    if(processId == 0)
        throw std::runtime_error("Couldn't find process with name " + std::string(process_name));

    return processId;
}
