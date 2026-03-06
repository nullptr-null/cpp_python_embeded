#pragma once

// define the name of the PythonBase module

#if defined(_DEBUG)
#define PYTHONBASE_MOD_NAME "python_base_d.pyd"
#else
#define PYTHONBASE_MOD_NAME "python_base.pyd"
#endif


#include "SimpleString.h"

#include "PythonBaseDefaultAllocator.h"


namespace PythonBase {
    using PyBaseString = SimpleString<char, PythonBaseDefaultAllocator<char>>;
}

