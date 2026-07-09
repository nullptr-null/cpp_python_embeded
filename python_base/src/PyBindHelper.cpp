#include "PyBindHelper.hpp"
#include <stdexcept>
#include <filesystem>
#include <cstdio>
#include <stdio.h>
#include <sstream>
#include "PythonBaseLogger.h"
#include <json.hpp>

// Python 头文件
#include <Python.h>
#include "PyInterpreterManager.hpp"

namespace core::python {

    std::string PyBindHelper::ExecuteBusiness(const std::string& json_input, 
                                            const std::string& python_script_path) {
        // Python 输出重定向包装器类
        class PythonStdoutRedirect {
        public:
            PythonStdoutRedirect(PyBindHelper* helper) : helper_(helper) {
                // 保存原始的 stdout
                old_stdout_ = PySys_GetObject("stdout");

                // 创建新的 StringIO 对象作为 stdout
                PyObject* io_module = PyImport_ImportModule("io");
                if (io_module) {
                    PyObject* stringio_class = PyObject_GetAttrString(io_module, "StringIO");
                    if (stringio_class) {
                        new_stdout_ = PyObject_CallObject(stringio_class, nullptr);
                        PySys_SetObject("stdout", new_stdout_);
                        Py_DECREF(stringio_class);
                    }
                    Py_DECREF(io_module);
                }
            }

            ~PythonStdoutRedirect() {
                // 获取所有输出
                if (new_stdout_) {
                    PyObject* output = PyObject_CallMethod(new_stdout_, "getvalue", nullptr);
                    if (output && PyUnicode_Check(output)) {
                        const char* output_str = PyUnicode_AsUTF8(output);
                        if (output_str) {
                            // 将输出分割成多行
                            std::istringstream iss(output_str);
                            std::string line;
                            while (std::getline(iss, line)) {
                                if (!line.empty()) {
                                    helper_->python_output_.push_back(line);
                                    // std::cout << "[Python] " << line << std::endl;
                                }
                            }
                        }
                        Py_DECREF(output);
                    }

                    // 恢复原始的 stdout
                    if (old_stdout_) {
                        PySys_SetObject("stdout", old_stdout_);
                    }
                    Py_DECREF(new_stdout_);
                }
            }

        private:
            PyBindHelper* helper_ = nullptr;
            PyObject* old_stdout_ = nullptr;
            PyObject* new_stdout_ = nullptr;
        };

        // 清空之前的输出
        python_output_.clear();

        // 检查脚本文件是否存在
        if (!std::filesystem::exists(python_script_path)) {
            throw std::runtime_error("Python script file not found: " + python_script_path);
        }

        // 必须先初始化GIL状态
        PyInterpreterManager::GILGuard gil_guard;

        std::string result;

        try {
            PythonStdoutRedirect redirector(this);

            auto self_module_lib_path = PyInterpreterManager::Instance().GetPythonSelfModuleLibPath();

            // 设置 Python 执行环境
            std::string setup_code = 
                R"(
import sys
import os
import json

sys.dont_write_bytecode = True

# 设置输入参数
input_json = r')" + json_input + R"('

# 添加脚本目录到路径
script_dir = r')" + std::filesystem::path(python_script_path).parent_path().string() + R"('
if script_dir and script_dir not in sys.path:
    sys.path.insert(0, script_dir)

print("Executing script:", r')" + python_script_path + R"(')
)";

            std::string self_lib_dir = R"(
self_lib_dir = r')" +
                    self_module_lib_path.string() + R"('
if self_lib_dir and self_lib_dir not in sys.path:
    sys.path.insert(0, self_lib_dir)
)";

            // debug setup_code 输出
            //PLOG_INFO << "Python setup code:\n" << setup_code;

            int ret = PyRun_SimpleString(setup_code.c_str());
            if (ret != 0) {
                PyErr_Print();
                throw std::runtime_error("Python string execution failed");
            }
            
            ret = PyRun_SimpleString(self_lib_dir.c_str());
            if (ret != 0) {
                PyErr_Print();
                throw std::runtime_error("Python string execution failed");
            }

            // 直接执行 Python 脚本
            FILE* script_file = nullptr;
            fopen_s(&script_file, python_script_path.c_str(), "r");
            if (!script_file) {
                throw std::runtime_error("Cannot open Python script: " + python_script_path);
            }

            ret = PyRun_SimpleFile(script_file, python_script_path.c_str());
            std::fclose(script_file);

            if (ret != 0) {
                PyErr_Print();
                throw std::runtime_error("Python script execution failed");
            }

            // 获取执行结果
            PyObject* main_module = PyImport_AddModule("__main__");
            PyObject* global_dict = PyModule_GetDict(main_module);
        
            PyObject* result_obj = PyDict_GetItemString(global_dict, "output_json");
            if (result_obj && PyUnicode_Check(result_obj)) {
                result = PyUnicode_AsUTF8(result_obj);
            } else {
                result = R"({"status": "fail", "message": "Script executed, no output_json found"})";
            }

        } catch (const std::exception&) {
            //throw;
            //  捕获异常，返回失败结果
            result = R"({"status": "fail", "message": "Exception occurred during Python execution"})";
        }

        // output the captured python output for debugging
        for (const auto& line : python_output_) {
            PythonBase::Logger::GetInstance()->info("[Python] " + line);
        }
        return result;
    }
    PyExecResult::PyExecResult(const std::string& result) {
        try {
            auto json_result = nlohmann::json::parse(result);
            if (json_result.contains("status") && json_result["status"] == "success") {
                success_ = true;
            } else {
                success_ = false;
            }
        } catch (const std::exception& e) {
            std::string error_message = "Failed to parse Python execution result: " + std::string(e.what());
            PythonBase::Logger::GetInstance()->error(error_message);
            success_ = false;
        }
    }
    bool PyExecResult::is_success() const {
        return success_;
    }
    }  // namespace core::python
