#include "Process.h"
#include <string>
#include <memory>

#pragma once
class Injector
{
public:
    Injector(const std::string processName, std::string libraryPath);
    ~Injector();

    void Inject();
    void Eject();
    bool IsInjected();
private:
    Process m_pProcess;
    int m_threadId;
    bool m_injected;
    std::string m_libraryPath;
    HMODULE m_kernel32ModuleHandle;
    HMODULE m_moduleToInjectHandle;

};

