#pragma once

#include "PythonBaseDefs.h"

#include <memory>
#include <string>
#include <filesystem>
#include <windows.h>

namespace PythonBase
{
    class DllLoader {
    public:
        DllLoader(const std::string& pluginPath) {
            try {
                if (pluginPath.empty()) {
                    m_bIsLoad = false;
                    return;
                }

#ifdef _WIN32
                m_hDLL = LoadLibraryA(pluginPath.c_str());
#else
                m_hDLL = dlopen(pluginPath.c_str(), RTLD_LAZY);
#endif
                if (nullptr == m_hDLL) {
                    m_bIsLoad = false;
                    return;
                }

                m_bIsLoad = true;
            } catch (...) {
            }
        }

        ~DllLoader() {
            try {
                if (m_bIsLoad) {
#ifdef _WIN32
                    if (m_hDLL) {
                        FreeLibrary((HMODULE)m_hDLL);
                    }
#else
                    if (m_hDLL) {
                        dlclose(m_hDLL);
                    }
#endif
                    m_hDLL = nullptr;  // Reset the DLL handle
                    m_bIsLoad = false;  // Mark the plugin as unloaded
                }
            } catch (...) {
            }
        }

        int Init() {
            try {
                if (!m_bIsLoad) {
                    return -1;  // Plugin not loaded
                }

                if (nullptr == m_hDLL) {
                    return -2;
                }

                using PLUGINAPI_INIT = int (*)();
                PLUGINAPI_INIT lpDllFunc = nullptr;
#ifdef _WIN32
                lpDllFunc = (PLUGINAPI_INIT)GetProcAddress((HMODULE)m_hDLL, "Init");
#else
                lpDllFunc = (PLUGINAPI_INIT)dlsym(m_hDLL, "Init");
#endif
                if (nullptr == lpDllFunc) {
                    return -2;
                }

                int nRet = lpDllFunc();

                return nRet;
            } catch (...) {
                return -2;
            }
        }
        int Deinit() {
            try {
                if (!m_bIsLoad) {
                    return -1;  // Plugin not loaded
                }
                if (nullptr == m_hDLL) {
                    return -2;
                }

                using PLUGINAPI_DEINIT = int (*)();
                PLUGINAPI_DEINIT lpDllFunc = nullptr;
#ifdef _WIN32
                lpDllFunc = (PLUGINAPI_DEINIT)GetProcAddress((HMODULE)m_hDLL, "Deinit");
#else
                lpDllFunc = (PLUGINAPI_DEINIT)dlsym(m_hDLL, "Deinit");
#endif
                if (nullptr == lpDllFunc) {
                    return -2;
                }

                int nRet = lpDllFunc();

                return nRet;
            } catch (...) {
                return -2;
            }
        }

        void* GetFunction(const std::string& funcName) {
            try {
                if (!m_bIsLoad) {
                    return nullptr;  // Plugin not loaded
                }
                if (nullptr == m_hDLL) {
                    return nullptr;
                }

#ifdef _WIN32
                void* pFunc = (void*)GetProcAddress((HMODULE)m_hDLL, funcName.c_str());
#else
                void* pFunc = (void*)dlsym(m_hDLL, funcName.c_str());
#endif
                return pFunc;

            } catch (...) {
                return nullptr;
            }
        }

        static void* GetFunction(void*  hDll, const std::string& funcName) {
            try {
                if (nullptr == hDll) {
                    return nullptr;
                }

#ifdef _WIN32
                void* pFunc = (void*)GetProcAddress((HMODULE)hDll, funcName.c_str());
#else
                void* pFunc = (void*)dlsym(hDll, funcName.c_str());
#endif
                return pFunc;

            } catch (...) {
                return nullptr;
            }
        }

        bool IsLoaded() const {
            return m_bIsLoad;
        }

    private:
        void* m_hDLL = nullptr;  // Handle to the loaded DLL
        bool m_bIsLoad = false;  // Flag to indicate if the plugin is loaded successfully
    };
};

