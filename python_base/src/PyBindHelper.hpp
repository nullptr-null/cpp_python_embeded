#ifndef PY_BIND_HELPER_H
#define PY_BIND_HELPER_H

#include <string>
#include <list>

namespace core::python {

    //class PyEnvHelper {
    //public:
    //    PyEnvHelper();
    //    ~PyEnvHelper();
    //};

    class PyExecResult
    {
    public:
        PyExecResult(const std::string& result);
        ~PyExecResult() = default;
        bool is_success() const;

    private:
        bool success_{false};
    };
    class PyBindHelper {
    public:
        PyBindHelper() = default;
        ~PyBindHelper() = default;

        std::string ExecuteBusiness(const std::string& json_input, 
                                   const std::string& python_script_path);

    private:
        std::list<std::string> python_output_;
    };

}  // namespace core::python

#endif // PY_BIND_HELPER_H
