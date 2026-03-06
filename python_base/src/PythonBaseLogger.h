#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "PythonBase.h"

namespace PythonBase {
class Logger : public IPythonBaseInterfaceLog {
public:
    void logInit(const std::string& moduleName, const std::string& configPath) {
        // Initialize logger with module name and config path
    }
    void info(const std::string& message) {
        if (m_provider) {
            m_provider->Log(PythonBaseLogType::PythonBaseLogType_Info, PyBaseString(message.c_str()));
        }
    }
    void error(const std::string& message) {
        if (m_provider) {
            m_provider->Log(PythonBaseLogType::PythonBaseLogType_Error, PyBaseString(message.c_str()));
        }
    }
    virtual void SetLogProvider(ILogProvider* provider) override {
        m_provider = provider;
    }
    virtual ~Logger() {}

    static std::shared_ptr<Logger> GetInstance() {
        static std::shared_ptr<Logger> instance(new Logger());
        return instance;
    }

private:
    // Private members for logger implementation
    ILogProvider* m_provider = nullptr;
};
}  // namespace PythonBase
