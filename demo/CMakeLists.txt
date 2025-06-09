cmake_minimum_required(VERSION 3.19)
project(demo)

set(CMAKE_CXX_STANDARD 20)

add_executable(demo main.cpp REPL.cpp)

target_link_libraries(demo PRIVATE polishd)

target_include_directories(demo PRIVATE
	${PROJECT_SOURCE_DIR}/polishd
)
