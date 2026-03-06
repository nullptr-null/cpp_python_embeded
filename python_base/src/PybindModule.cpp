#include <pybind11/pybind11.h>
#include "PyBindHelper.hpp"
#include "PythonBaseUIViewLogger.h"
namespace py = pybind11;

using namespace core::python;

 namespace py = pybind11;
 class log_viewer {
 public:
     void post_error(const std::string& content) {
         PythonBase::UIViewLogger::GetInstance()->post_error(content);
     }
     void post_info(const std::string& content) {
         PythonBase::UIViewLogger::GetInstance()->post_info(content);
     }

     void post_warning(const std::string& content) {
         PythonBase::UIViewLogger::GetInstance()->post_warning(content);
     }
 };

 PYBIND11_MODULE(python_base, m) {
     m.doc() = "GUI Log Viewer Module";
     py::class_<log_viewer>(m, "log_viewer")
             .def(py::init<>())
             .def("post_error", &log_viewer::post_error, py::arg("content"))
             .def("post_info", &log_viewer::post_info, py::arg("content"))
             .def("post_warning", &log_viewer::post_warning, py::arg("content"));
 }
