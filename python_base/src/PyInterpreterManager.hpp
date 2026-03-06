#pragma once
#include <mutex>
#include <Python.h>
#include <filesystem>

namespace core::python {

class PyInterpreterManager {
public:

    static PyInterpreterManager& Instance();

    // Idempotent, thread-safe init
    void Init();

    // Thread-safe finalize. Caller must ensure no other threads will use Python.
    void Finalize();

    // RAII GIL guard for callers that need to run Python code.
    class GILGuard {
    public:
        GILGuard() { state_ = PyGILState_Ensure(); }
        ~GILGuard() { PyGILState_Release(state_); }
    private:
        PyGILState_STATE state_;
    };


    std::filesystem::path GetPythonSelfModuleLibPath() const {
        return m_pySelfModLibPath;
    }

private:

    PyInterpreterManager() = default;
    ~PyInterpreterManager() {
        Finalize();
    }

    PyInterpreterManager(const PyInterpreterManager&) = delete;
    PyInterpreterManager& operator=(const PyInterpreterManager&) = delete;

    std::once_flag init_flag_;
    std::mutex fini_mutex_;
    bool initialized_ = false;
    PyThreadState* poPyThreadStateSave = nullptr;
    std::filesystem::path m_pySelfModLibPath;
};

} // namespace core::python