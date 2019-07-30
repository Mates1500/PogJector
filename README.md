# PogJector
A simple DLL Injector

## How to use
1. Create and compile a DLL to be injected. Put the injection code into the [DllMain()](https://docs.microsoft.com/en-us/windows/win32/dlls/dllmain) entry point.
2. Choose whether to alter your debug privileges. Some processes (especially protected games) require these privileges for [OpenProcess()](https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-openprocess) to succeed
3. Enter the running process name exactly as it appears in Windows (case-sensitive, might improve this later)
4. Enter the path to the compiled DLL for injection

## Improvements to be made
* argc, argv support for easier launching from external utilities
* ignore case sensitivity for process names
