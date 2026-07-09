#pragma once

#include "SimpleString.h"
#include "PythonBaseDefs.h"
#include "PythonBaseDefaultAllocator.h"
#include "PythonBaseDllLoader.h"

#include <memory>
#include <string>
#include <filesystem>
#include <windows.h>

namespace PythonBase
{
    enum class PythonBaseInterfaceType
    {
        PYTHONBASE_INTERFACE_ENV_MGR = 0, //
        PYTHONBASE_INTERFACE_PY_MGR = 1,  //
        PYTHONBASE_INTERFACE_UIVIEW_LOG = 2,  //
        PYTHONBASE_INTERFACE_LOG = 3,  //
        PYTHONBASE_INTERFACE_TYPE_MAX,
    };

    class PythonBaseInterfaceBase {
    public:
        virtual ~PythonBaseInterfaceBase() {}
    };

#ifndef PYTHONBASE_EXPORTS

    class PythonBase
    {
    public:
        PythonBase(const std::string& pyevn_path) {
            m_dllLoaderMgr = nullptr;

            if (std::filesystem::exists(pyevn_path)) {
                SetDllDirectoryA(pyevn_path.c_str());
            }
            else
            {
                throw std::runtime_error("Python environment path does not exist: " + pyevn_path);
            }
            std::string dllPath = pyevn_path + "/" + PYTHONBASE_MOD_NAME;
            auto dllLoaderMgr = std::make_shared<DllLoader>(dllPath);
            if (dllLoaderMgr == nullptr) {
                throw std::runtime_error("Failed to create DllLoader for PythonBase.dll");
            }
            else
            {
                if (!dllLoaderMgr->IsLoaded())
                {
                    throw std::runtime_error("Failed to load PythonBase.dll from path: " + dllPath);
                }
            }
            m_dllLoaderMgr = dllLoaderMgr;
        }

        ~PythonBase() {
            m_dllLoaderMgr = nullptr;
        }

        int PythonBase_Init() {
            if (m_dllLoaderMgr == nullptr) {
                return -1;
            }
            int ret = m_dllLoaderMgr->Init();
            return ret;
        }

        int PythonBase_Deinit() {
            if (m_dllLoaderMgr != nullptr) {
                int ret = m_dllLoaderMgr->Deinit();
                return ret;
            }
            return -1;
        }

        int PythonBase_CreateInterface(PythonBaseInterfaceType type, void** obj) {
            if (m_dllLoaderMgr == nullptr) {
                return -1;
            }

            using CREATEINTERFACE = int (*)(PythonBaseInterfaceType, void**);
            CREATEINTERFACE pFunc = (CREATEINTERFACE)m_dllLoaderMgr->GetFunction("CreateInterface");
            if (pFunc == nullptr) {
                return -2;
            }
            auto ret = pFunc(type, obj);
            return ret;
        }

        int PythonBase_DestroyInterface(PythonBaseInterfaceType type, void* obj) {
            if (m_dllLoaderMgr == nullptr) {
                return -1;
            }
            using DESTROYINTERFACE = int (*)(PythonBaseInterfaceType, void*);
            DESTROYINTERFACE pFunc = (DESTROYINTERFACE)m_dllLoaderMgr->GetFunction("DestroyInterface");
            if (pFunc == nullptr) {
                return -2;
            }
            auto ret = pFunc(type, obj);

            return ret;
        }

        template <typename T>
        std::shared_ptr<T> GetPythonBaseInterface(PythonBaseInterfaceType type) {
            void* base = nullptr;
            if (PythonBase_CreateInterface(type, &base) != 0) {
                return nullptr;
            }
            if (base == nullptr) {
                return nullptr;  // Failed to create interface
            }
            T* typePtr = static_cast<T*>(base);

            if (typePtr == nullptr) {
                return nullptr;  // Failed to create interface
            }

            std::shared_ptr<T> interfacePtr =
                    std::shared_ptr<T>(typePtr, [type,this](T* ptr) { PythonBase_DestroyInterface(type, ptr); });
            return interfacePtr;
        }
    private:
        std::shared_ptr<DllLoader> m_dllLoaderMgr;
    };

#endif

// API functions
    class IPythonBaseInterfaceEnvMgr : public PythonBaseInterfaceBase {
    public:
        virtual ~IPythonBaseInterfaceEnvMgr() {}
    };
    class IPythonBaseInterfacePyMgr : public PythonBaseInterfaceBase {
    public:
        virtual ~IPythonBaseInterfacePyMgr() {}
        virtual PyBaseString ExecutePy(const PyBaseString& json_input, const PyBaseString& python_script_path) = 0;
    };

    enum class PythonBaseLogType {
        PythonBaseLogType_Info = 0,  //
        PythonBaseLogType_Warning = 1,  //
        PythonBaseLogType_Error = 2,  //
    };

    class ILogProvider {
        public:
        virtual ~ILogProvider() {}
            virtual void Log(PythonBaseLogType type, const PyBaseString& message) = 0;
    };

    class IPythonBaseInterfaceLog : public PythonBaseInterfaceBase {
    public:
        virtual ~IPythonBaseInterfaceLog() {}
        virtual void SetLogProvider(ILogProvider* provider) = 0;
    };

    class IPythonBaseInterfaceUIViewLog : public PythonBaseInterfaceBase {
    public:
        virtual ~IPythonBaseInterfaceUIViewLog() {}
        virtual void SetLogProvider(ILogProvider* provider) = 0;
    };


#ifndef PYTHONBASE_EXPORTS

    inline std::shared_ptr<IPythonBaseInterfaceEnvMgr> GetPythonBaseEnvMgrInterface(PythonBase& pybase)
    {
        return pybase.GetPythonBaseInterface<IPythonBaseInterfaceEnvMgr>(
                PythonBaseInterfaceType::PYTHONBASE_INTERFACE_ENV_MGR);
    }
    inline std::shared_ptr<IPythonBaseInterfacePyMgr> GetPythonBasePyMgrInterface(PythonBase& pybase)
    {
        return pybase.GetPythonBaseInterface<IPythonBaseInterfacePyMgr>(
                PythonBaseInterfaceType::PYTHONBASE_INTERFACE_PY_MGR);
    }
    inline std::shared_ptr<IPythonBaseInterfaceUIViewLog> GetPythonBaseUIViewLogInterface(PythonBase& pybase)
    {
        return pybase.GetPythonBaseInterface<IPythonBaseInterfaceUIViewLog>(
                PythonBaseInterfaceType::PYTHONBASE_INTERFACE_UIVIEW_LOG);
    }
    inline std::shared_ptr<IPythonBaseInterfaceLog> GetPythonBaseLogInterface(PythonBase& pybase)
    {
        return pybase.GetPythonBaseInterface<IPythonBaseInterfaceLog>(
                PythonBaseInterfaceType::PYTHONBASE_INTERFACE_LOG);
    }
#endif
};

