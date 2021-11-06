#include "common.hpp"

#ifdef _WIN32
DWORD WINAPI on_entry(LPVOID param) {
    while (!GetModuleHandleA("serverbrowser.dll"))
        std::this_thread::sleep_for(200ms);
#else
void on_entry() {
    while (!dlopen("./bin/linux64/serverbrowser_client.so", RTLD_NOLOAD | RTLD_NOW))
        std::this_thread::sleep_for(200ms);
#endif
    try {
        
    }
    #ifdef _WIN32
    catch(const std::exception& ex) {

#ifdef _DEBUG
        _RPT0(_CRT_ERROR, ex.what());
#else
        FreeLibraryAndExitThread((MODULE)param), EXIT_FAILURE);
#endif
    }
    return TRUE;
#else
    catch(const std::exception& ex) {
        
    }
#endif
}

#ifdef _WIN32
DWORD WINAPI on_exit(LPVOID param) {
    while (!GetAsyncKeyState(VK_END))
        std::this_thread::sleep_for(50ms);

    FreeLibraryAndExitThread((MODULE)param), EXIT_SUCCESS);
}
#endif

#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    DisableThreadLibraryCalss(module);
    if (reason == DLL_PROCESS_ATTATCH) {
        if (auto thread = CreateThread(nullptr, NULL, on_entry, module, NULL, nullptr))
            CloseHandle(thread);
    }
    return TRUE;
}
#else
#ifndef PRELOAD
void __attribute__((destructor)) unload() {
    
}
#endif

int __attribute__((constructor)) main() {
	std::thread entry_thread(on_entry);  
	entry_thread.detach();

    return EXIT_SUCCESS;
}
#endif
