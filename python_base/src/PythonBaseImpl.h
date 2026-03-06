#pragma once

#include "PythonBase.h"
#include <memory>

#ifdef PYTHONBASE_API
// only imported by getprocaddress

#else
    #ifdef _WIN32
        #ifdef PYTHONBASE_EXPORTS
            #define PYTHONBASE_API __declspec(dllexport)
        #else
            #define PYTHONBASE_API
        #endif
    #else
        #define PYTHONBASE_API __attribute__((visibility("default")))
#endif
#endif

namespace PythonBase {

extern "C" PYTHONBASE_API int Init();
extern "C" PYTHONBASE_API int Deinit();
extern "C" PYTHONBASE_API int CreateInterface(PythonBaseInterfaceType type, PythonBaseInterfaceBase** obj);
extern "C" PYTHONBASE_API int DestroyInterface(PythonBaseInterfaceType type, PythonBaseInterfaceBase* obj);
extern "C" PYTHONBASE_API void* MmAlloc(size_t size);

extern "C" PYTHONBASE_API void MmFree(void* ptr);
}  // namespace PythonBase

