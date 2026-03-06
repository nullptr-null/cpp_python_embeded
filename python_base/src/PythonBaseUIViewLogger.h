#pragma once
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "PythonBase.h"

namespace PythonBase {
class UIViewLogger : public IPythonBaseInterfaceUIViewLog {
public:
    ~UIViewLogger()
    {

    }
    void post_error(const std::string& content) {
        // Placeholder implementation
        if (m_provider) {
            m_provider->Log(PythonBaseLogType::PythonBaseLogType_Error, PyBaseString(content.c_str()));
        }

    }
    void post_info(const std::string& content) {
        // Placeholder implementation
        if (m_provider) {
            m_provider->Log(PythonBaseLogType::PythonBaseLogType_Info, PyBaseString(content.c_str()));
        }
    }

    void post_warning(const std::string& content) {
        // Placeholder implementation
        if (m_provider) {
            m_provider->Log(PythonBaseLogType::PythonBaseLogType_Warning, PyBaseString(content.c_str()));
        }
    }
    virtual void SetLogProvider(ILogProvider* provider) override {
        m_provider = provider;
    }

    static std::shared_ptr<UIViewLogger> GetInstance() {
        static std::shared_ptr<UIViewLogger> instance(new UIViewLogger());
        return instance;
    }

private:
    // Private members for logger implementation
    ILogProvider* m_provider = nullptr;
};
}  // namespace PythonBase
