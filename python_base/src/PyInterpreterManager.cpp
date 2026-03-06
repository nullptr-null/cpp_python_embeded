#include "PyInterpreterManager.hpp"
#include "PythonBaseDefs.h"
#include <filesystem>
#include <Windows.h>

namespace core::python {

    PyInterpreterManager& PyInterpreterManager::Instance() {
    static PyInterpreterManager instance;
    return instance;
    }

    void PyInterpreterManager::Init() {
        std::call_once(init_flag_, [this]() {

            // get current dll path
            HMODULE mod = ::GetModuleHandleA(PYTHONBASE_MOD_NAME);
            if (mod == nullptr) {
                return;
            }

            char dll_path_str[MAX_PATH];
            if (GetModuleFileNameA(mod, dll_path_str, MAX_PATH) == 0) {
                // Handle error
                return;
            }

            std::filesystem::path exe_path(dll_path_str);

            auto py_env_path = exe_path.parent_path();
            m_pySelfModLibPath = py_env_path;
            if (std::filesystem::exists(py_env_path)) {
                PyStatus status;
                PyConfig config;

                PyConfig_InitIsolatedConfig(&config);  // 隔离模式

                status = PyConfig_SetString(&config, &config.home,
                                            py_env_path.wstring().c_str());  // Windows

                config.isolated = 1;
                config.use_environment = 0;

                wchar_t host_path_str[MAX_PATH] = {0};
                if (GetModuleFileName(nullptr, host_path_str, MAX_PATH) != 0) {
                    config.program_name = (wchar_t*)host_path_str;
                }


                status = Py_InitializeFromConfig(&config);
                //if (PyStatus_Exception(status)) {
                //    PyConfig_Clear(&config);
                //    return;
                //}
            }
            else
            {
                // 初始化解释器并创建 GIL
                Py_Initialize();
            }

            // release GIL so other threads may acquire it
            poPyThreadStateSave = PyEval_SaveThread();
            initialized_ = true;
        });
    }

void PyInterpreterManager::Finalize() {
    std::lock_guard<std::mutex> lk(fini_mutex_);
    if (!initialized_ || !Py_IsInitialized()) return;

    PyEval_RestoreThread(poPyThreadStateSave);
    Py_FinalizeEx();
    initialized_ = false;
}

} // namespace core::python