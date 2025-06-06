cmake_minimum_required(VERSION 3.19)
project(polishd)

set(CMAKE_CXX_STANDARD 20)

add_executable(polishd main.cpp Compiler.hpp Function.hpp Token.hpp Grammar.hpp Calculator.hpp)
