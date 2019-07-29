#include <Windows.h>
#include "Win32Handle.h"

#pragma once
class DebugPrivilegeGranter
{
public:
    DebugPrivilegeGranter();
private:
    Win32Handle m_processHandle;
    Win32Handle m_processTokenHandle;
    TOKEN_PRIVILEGES m_privilegeToken;
};

