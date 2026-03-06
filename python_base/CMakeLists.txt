
file(GLOB_RECURSE PYTHON_BASE_INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/interface/*.h")
file(GLOB_RECURSE PYTHON_BASE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PYTHON_BASE_INTERFACE})
#source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${PYTHON_BASE_SOURCES})

# Create executable
add_library(python_base SHARED
    ${PYTHON_BASE_INTERFACE}
    ${PYTHON_BASE_SOURCES}
)


# Private include directories only for current target
target_include_directories(python_base PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/interface
    ${CMAKE_SOURCE_DIR}/deps/third_party/json-3.11.3
    ${CMAKE_SOURCE_DIR}/deps/third_party/fmt-11.0.2/include
    ${CMAKE_SOURCE_DIR}/deps/third_party/pybind11-3.0.1/include
)

# 查找 Python
set(Python_ROOT_DIR ${CMAKE_SOURCE_DIR}/deps/third_party/python-3.12.4)
find_package(Python REQUIRED COMPONENTS Development)
target_link_libraries(python_base PRIVATE Python::Python)


# Set common options
set_common_options(python_base)

set_target_properties(python_base PROPERTIES FOLDER ${FRAMEWORK_FOLDER})

if(WIN32)
    set_target_properties(python_base PROPERTIES 
        SUFFIX ".pyd"
        PREFIX ""
        DEBUG_POSTFIX "_d")
endif()

get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

# Set output directories
if (is_multi_config)
    set_target_properties(python_base
        PROPERTIES
            # Windows: 控制 .dll 文件的位置
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>/py_env"
            # Windows: 控制 .lib 导入库的位置 (Linux/macOS 上此属性对静态库有效)
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>/py_env"
            # Linux/macOS: 控制 .so/.dylib 文件的位置
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>/py_env"
    )
else()
    set_target_properties(python_base
        PROPERTIES
            # Windows: 控制 .dll 文件的位置
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/py_env"
            # Windows: 控制 .lib 导入库的位置 (Linux/macOS 上此属性对静态库有效)
            ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/py_env"
            # Linux/macOS: 控制 .so/.dylib 文件的位置
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/py_env"
    )
endif()

# Preprocessor definitions
# add Py_DEBUG definition so as to solve link error in debug mode
target_compile_definitions(python_base PRIVATE
    $<$<CONFIG:Debug>:Py_DEBUG>
    PYTHONBASE_EXPORTS
)

# Copy to py_env.
add_custom_command(TARGET python_base POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:python_base>
    ${CMAKE_SOURCE_DIR}/py_env/$<IF:$<CONFIG:Debug>,Debug,Release>
)

