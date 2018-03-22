cmake_minimum_required(VERSION 3.9)
project(lmfpy)

set(CMAKE_CXX_STANDARD 17)

find_package(pybind11 REQUIRED)
add_subdirectory(fmt)

add_library(lmfpy SHARED LMF_IO.h LMF_IO.cpp binder.h binder.cpp)
target_link_libraries(lmfpy
        PRIVATE pybind11::module
        PRIVATE fmt::fmt-header-only)
set_target_properties(lmfpy
        PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
        SUFFIX "${PYTHON_MODULE_EXTENSION}")
