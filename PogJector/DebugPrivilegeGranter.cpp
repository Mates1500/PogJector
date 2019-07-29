#include "DebugPrivilegeGranter.h"
#include <stdexcept>
#include <string>

DebugPrivilegeGranter::DebugPrivilegeGranter()
    :m_processHandle(GetCurrentProcess()),
    m_processTokenHandle(nullptr)
{
    HANDLE processTokenHandleForInit = nullptr;
    bool processTokenAcquireSuccess = OpenProcessToken(m_processHandle.get(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &processTokenHandleForInit);
    m_processTokenHandle.reset(processTokenHandleForInit);
    if (!processTokenAcquireSuccess)
        throw std::runtime_error("DebugPrivilegeGranter::DebugPrivilegeGranter - Failed to open process token, error id:" + std::to_string(GetLastError()));
    LookupPrivilegeValue(0, SE_DEBUG_NAME, &m_privilegeToken.Privileges[0].Luid);
    m_privilegeToken.PrivilegeCount = 1;
    m_privilegeToken.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    bool tokenAdjustmentResult = AdjustTokenPrivileges(m_processTokenHandle.get(), 0, &m_privilegeToken, sizeof(TOKEN_PRIVILEGES), 0, 0);
    if (!tokenAdjustmentResult)
        throw std::runtime_error("DebugPrivilegeGranter::DebugPrivilegeGranter - Failed to adjust token privileges (check that the program is running as Admin), error id:" + std::to_string(GetLastError()));
}
