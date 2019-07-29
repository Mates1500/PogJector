#include <iostream>
#include <string>
#include <stdexcept>
#include "Process.h"
#include "Injector.h"
#include "DebugPrivilegeGranter.h"

int main()
{
    char useDebug = 0;
    while (toupper(useDebug) != 'Y' && toupper(useDebug) != 'N')
    {
            std::cout << "Use debug privileges? Make sure to run as Admin first! (Y/N)";
            std::cin >> useDebug;
    }
    if (toupper(useDebug) == 'Y')
        DebugPrivilegeGranter privilegeGranter = DebugPrivilegeGranter();

    std::string processName;
    std::cout << "Enter process name:" << std::endl;
    std::cin >> processName;
    

    std::string dllPath;
    std::cout << "Enter dll path:" << std::endl;
    std::cin >> dllPath;
    
    try
    {
        Injector injector(processName, dllPath);
        injector.Inject();

        std::cout << "Library injected. Press any key to eject..." << std::endl;
        getchar(); getchar();
        injector.Eject();
        std::cout << "Injected library ejected. Press any key to exit..." << std::endl;
        getchar();
    }
    catch (std::runtime_error e)
    {
        std::cout << "An exception has been raised: " << e.what() << std::endl;
        std::cout << "Press any key to exit...";
        getchar(); getchar();
    }
}
