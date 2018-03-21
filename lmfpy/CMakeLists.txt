cmake_minimum_required(VERSION 3.7)
project(lmfpy)

set(CMAKE_CXX_STANDARD 14)

find_package(pybind11 REQUIRED)

add_library(lmfcpp STATIC LMF_IO.h LMF_IO.cpp LMFReader.h LMFReader.cpp)
pybind11_add_module(lmfpy MODULE LMF_IO.h LMF_IO.cpp LMFReader.h LMFReader.cpp binder.cpp)
