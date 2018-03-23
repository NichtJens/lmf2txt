cmake_minimum_required(VERSION 3.9)
project(lmfpy)

set(CMAKE_CXX_STANDARD 14)

find_package(pybind11 REQUIRED)

pybind11_add_module(lmfpy LMF_IO.h LMF_IO.cpp binder.h binder.cpp)
