
#include "PythonBaseImpl.h"
#include "PythonBaseLogger.h"
#include "PythonBaseUIViewLogger.h"
#include "PyInterpreterManager.hpp"
#include "PyBindHelper.hpp"
#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace PythonBase 
{
    std::map<PythonBaseInterfaceType, std::vector<std::shared_ptr<PythonBaseInterfaceBase>>> g_pluginInterface;
    std::shared_ptr<Logger> g_logger = Logger::GetInstance();

    // mutex
    std::mutex g_interfaceMutex;

    class PythonBaseInterfaceEnvMgr : public IPythonBaseInterfaceEnvMgr {
    public:
    };
    class PythonBaseInterfacePyMgr : public IPythonBaseInterfacePyMgr {
    public:
        PyBaseString ExecutePy(const PyBaseString& json_input, const PyBaseString& python_script_path) override
        {
            core::python::PyBindHelper pyHelper;
            std::string result = pyHelper.ExecuteBusiness(json_input.c_str(), python_script_path.c_str());
            return PyBaseString(result.c_str());
        }
    };

    PYTHONBASE_API int Init()
    {

        g_logger->logInit("PythonBase", "conf/log_config.json");
        g_logger->info("start...");

        core::python::PyInterpreterManager::Instance().Init();
        return 0;
    }

    PYTHONBASE_API int Deinit()
    {
        // 清理插件
        core::python::PyInterpreterManager::Instance().Finalize();

        return 0;
    }

    PYTHONBASE_API int CreateInterface(PythonBaseInterfaceType type, PythonBaseInterfaceBase** obj)
    {
        std::lock_guard<std::mutex> lock(g_interfaceMutex);
        if (type < PythonBaseInterfaceType::PYTHONBASE_INTERFACE_ENV_MGR ||
            type >= PythonBaseInterfaceType::PYTHONBASE_INTERFACE_TYPE_MAX)
        {
            g_logger->error("PythonBase_CreateInterface: Invalid interface type.");
            return -1; // Invalid type
        }

        if (obj == nullptr)
        {
            g_logger->error("PythonBase_CreateInterface: Interface pointer is null.");
            return -1; // Invalid interface pointer
        }

        // Check if the interface already exists
        if (g_pluginInterface[type].empty())
        {
            // Create a new interface instance

            if (type == PythonBaseInterfaceType::PYTHONBASE_INTERFACE_ENV_MGR)
            {
                g_logger->info("Creating Environment Manager interface instance.");
                auto newInterface = std::make_shared<PythonBaseInterfaceEnvMgr>();
                g_pluginInterface[type].push_back(newInterface);
                *obj = newInterface.get();  // Set the output pointer to the raw pointer of the shared_ptr
            } else if (type == PythonBaseInterfaceType::PYTHONBASE_INTERFACE_PY_MGR)
            {
                g_logger->info("Creating Python Manager interface instance.");
                auto newInterface = std::make_shared<PythonBaseInterfacePyMgr>();
                g_pluginInterface[type].push_back(newInterface);
                *obj = newInterface.get(); // Set the output pointer to the raw pointer of the shared_ptr               
            } else if (type == PythonBaseInterfaceType::PYTHONBASE_INTERFACE_UIVIEW_LOG)
            {
                g_logger->info("Creating UIVIEW_LOG interface instance.");
                auto newInterface = UIViewLogger::GetInstance();
                g_pluginInterface[type].push_back(newInterface);
                *obj = newInterface.get(); // Set the output pointer to the raw pointer of the shared_ptr               
            } else if (type == PythonBaseInterfaceType::PYTHONBASE_INTERFACE_LOG)
            {
                g_logger->info("Creating LOG interface instance.");
                auto newInterface = g_logger;
                g_pluginInterface[type].push_back(newInterface);
                *obj = newInterface.get(); // Set the output pointer to the raw pointer of the shared_ptr               
            }
            else
            {
                g_logger->error("PythonBase_CreateInterface: Unsupported interface type.");
                return -1; // Unsupported type
            }
        }
        else
        {
            // If an interface already exists, return the first one
            auto existsIf = g_pluginInterface[type][0];
            *obj = existsIf.get(); // Set the output pointer to the raw pointer of the shared_ptr
            g_pluginInterface[type].push_back(existsIf); // 单例接口
        }
        return 0; // Success
    }
    PYTHONBASE_API int DestroyInterface(PythonBaseInterfaceType type, PythonBaseInterfaceBase* obj)
    {
        std::lock_guard<std::mutex> lock(g_interfaceMutex);
        if (obj == nullptr)
        {
            g_logger->error("PythonBase_DestroyInterface: Interface is null.");
            return -1; // Invalid interface
        }

        if (type < PythonBaseInterfaceType::PYTHONBASE_INTERFACE_ENV_MGR ||
            type >= PythonBaseInterfaceType::PYTHONBASE_INTERFACE_TYPE_MAX)
        {
            g_logger->error("PythonBase_DestroyInterface: Invalid interface type.");
            return -1; // Invalid type
        }

        // Find the interface in the map
        auto& interfaces = g_pluginInterface[type];
        if (!interfaces.empty())
        {
            // remove any one;
            interfaces.erase(interfaces.begin());
        }

        return -1; // Interface not found
    }

    PYTHONBASE_API void* MmAlloc(size_t size)
    {
        return PythonBase_Alloc(size);
    }

    PYTHONBASE_API void MmFree(void* ptr)
    {
        PythonBase_Free(ptr);
    }

}; // namespace PythonBase
