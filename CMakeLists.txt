cmake_minimum_required(VERSION 3.7)
project(lmf2txt)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(lmfpy)

add_executable(lmf2txt lmf2txt.cpp)
target_link_libraries(lmf2txt lmfcpp)
