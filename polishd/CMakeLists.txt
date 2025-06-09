cmake_minimum_required(VERSION 3.19)
project(polishd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(polishd STATIC Token.hpp Grammar.cpp Function.cpp Compiler.cpp)

target_include_directories(polishd PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(polishd PRIVATE -Wall -Wextra -Wpedantic)
