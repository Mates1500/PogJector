#include "Injector.h"
#include <stdexcept>

constexpr auto WINAPI_FREELIBRARY = "FreeLibrary";
constexpr auto WINAPI_LOADLIBRARY = "LoadLibraryA";;

Injector::Injector(std::string processName, std::string libraryPath)
    :m_pProcess(Process(processName)),
    m_libraryPath(libraryPath),
    m_injected(false),
    m_kernel32ModuleHandle(GetModuleHandle("kernel32.dll")),
    m_moduleToInjectHandle(NULL),
    m_threadId(0)
{
    if (m_kernel32ModuleHandle == NULL)
        throw new std::runtime_error("Injector::Injector - Failed to initialize kernel32.dll module handle");
}

Injector::~Injector()
{
    if (m_injected)
        Eject();
}

void Injector::Inject()
{
    if (GetFileAttributesA(m_libraryPath.c_str()) == INVALID_FILE_ATTRIBUTES)
        throw std::runtime_error("Injection::Inject - Could not find library at " + m_libraryPath);

    FARPROC loadLibraryAddress = GetProcAddress(m_kernel32ModuleHandle, WINAPI_LOADLIBRARY);

    int libraryPathLength = m_libraryPath.size() + 1; // +1 for null terminator
    LPVOID allocatedStringSpace = m_pProcess.AllocateMemory(libraryPathLength); 
    m_pProcess.WriteMemory(allocatedStringSpace, m_libraryPath.c_str(), libraryPathLength);
    m_moduleToInjectHandle = reinterpret_cast<HMODULE>(m_pProcess.CallFunction(loadLibraryAddress, allocatedStringSpace));
    m_pProcess.FreeMemory(allocatedStringSpace);


    m_injected = true;
}

void Injector::Eject()
{
    if (!m_injected)
        throw std::runtime_error("Injection::Eject - Nothing has been injected");

    FARPROC freeLibraryAddress = GetProcAddress(m_kernel32ModuleHandle, WINAPI_FREELIBRARY);
    m_pProcess.CallFunction(freeLibraryAddress, m_moduleToInjectHandle);
    m_injected = false;
}

bool Injector::IsInjected()
{
    return m_injected;
}
