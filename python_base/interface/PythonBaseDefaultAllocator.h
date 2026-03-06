#pragma once

#include "PythonBaseDefs.h"
#include "PythonBaseDllLoader.h"

#include <memory>
#include <string>
#include <filesystem>
#include <windows.h>

namespace PythonBase
{
#ifndef PYTHONBASE_EXPORTS

    inline void* PythonBase_Alloc(size_t size)
    {
        void* mod = ::GetModuleHandleA(PYTHONBASE_MOD_NAME);
        if (mod == nullptr) {
            return nullptr;
        }
        using MALLOCFUNC = void* (*)(size_t);
        MALLOCFUNC pFunc = (MALLOCFUNC)DllLoader::GetFunction(mod , "MmAlloc");
        if (pFunc == nullptr) {
            return nullptr;
        }
        return pFunc(size);
    }
    inline void PythonBase_Free(void* ptr)
    {
        void* mod = ::GetModuleHandleA(PYTHONBASE_MOD_NAME);
        if (mod == nullptr) {
            return;
        }
        using FREEFUNC = void (*)(void*);
        FREEFUNC pFunc = (FREEFUNC)DllLoader::GetFunction(mod, "MmFree");
        if (pFunc == nullptr) {
            return;
        }
        pFunc(ptr);
    }

#else
    inline void* PythonBase_Alloc(size_t size)
    {
        return malloc(size);
    }
    inline void PythonBase_Free(void* ptr)
    {
        free(ptr);
    }
#endif

    // 默认内存分配器
    template <typename T>
    struct PythonBaseDefaultAllocator {
        static T* allocate(size_t count) {
            return static_cast<T*>(PythonBase_Alloc(count * sizeof(T)));
        }

        static void deallocate(T* ptr) noexcept {
            PythonBase_Free(ptr);
        }

        template <typename... Args>
        static void construct(T* ptr, Args&&... args) {
            new (ptr) T(std::forward<Args>(args)...);
        }

        static void destroy(T* ptr) noexcept {
            ptr->~T();
        }
    };
};

