cmake_minimum_required(VERSION 3.19)
project(repl)

set(CMAKE_CXX_STANDARD 20)

add_executable(repl main.cpp REPL.cpp)

target_link_libraries(repl PRIVATE polishd)

target_include_directories(repl PRIVATE
	${PROJECT_SOURCE_DIR}/polishd
)
