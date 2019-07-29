#include <Windows.h>
#include <memory>

#pragma once

struct Win32HandleDeleter
{
    typedef HANDLE pointer;
    void operator()(HANDLE handle) noexcept
    {
        if (handle != INVALID_HANDLE_VALUE && handle != nullptr)
            CloseHandle(handle);
    }
};
typedef std::unique_ptr<HANDLE, Win32HandleDeleter> Win32Handle;