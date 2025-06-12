cmake_minimum_required(VERSION 3.19)
project(polishd LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_library(${PROJECT_NAME} STATIC TransparentStringKeyMap.hpp Token.hpp Grammar.cpp Function.cpp Compiler.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
