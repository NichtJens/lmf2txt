cmake_minimum_required(VERSION 3.9)
project(lmf2txt)

set(CMAKE_CXX_STANDARD 11)

add_executable(lmf2txt
        lmf2txt.cpp
        LMF_IO.cpp
        LMF_IO.h
        )
