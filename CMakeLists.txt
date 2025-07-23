cmake_minimum_required(VERSION 3.28)
project(code)

set(CMAKE_CXX_STANDARD 20)

add_executable(code
    src/main.cpp
        src/decoder.hpp
        src/memory.hpp
        src/base.hpp
        src/rs.hpp
        src/rob.hpp
        src/alu.hpp
        src/lsb.hpp
        src/coder.hpp
)
